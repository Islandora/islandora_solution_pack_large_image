<?php

/**
 * @file
 *
 * This file contains all admin functions
 */

/**
 * Admin form definition.
 */
function islandora_large_image_admin($form, &$form_state) {
  // include css
  drupal_add_css(drupal_get_path('module', 'islandora_large_image') . '/css/islandora_large_image.admin.css');

  if (isset($form_state['values']['islandora_kakadu_url'])) {
    $kakadu_url = $form_state['values']['islandora_kakadu_url'];
  }
  else {
    $kakadu_url = variable_get('islandora_kakadu_url', '/usr/bin/kdu_compress');
  }

  $lossless = variable_get('islandora_lossless', FALSE);

  module_load_include('inc', 'islandora_large_image', 'includes/utilities');
  $im_jp2000 = islandora_large_image_check_imagemagick_for_jpeg2000();

  $kakadu_checked = isset($form_state['values']['islandora_use_kakadu']) ? $form_state['values']['islandora_use_kakadu'] : variable_get('islandora_use_kakadu', $im_jp2000);
  $kakadu_view = $kakadu_checked ? 'markup' : 'hidden';


  exec($kakadu_url, $output, $return_value);
  $confirmation_message = '<img src="' . url('misc/watchdog-ok.png') . '"/>'
      . t('kakadu executable found at !kakadu_url', array('!kakadu_url' => "<strong>$kakadu_url</strong>"));
  if ($return_value != 0) {
    $confirmation_message = '<img src="' . url('misc/watchdog-error.png') . '"/> '
        . t('Unable to find kakadu executable at !kakadu_url', array('!kakadu_url' => "<strong>$kakadu_url</strong>"));
  }

  $form['islandora_lossless'] = array(
    '#title' => t("Lossless Derivative Creation?"),
    '#default_value' => $lossless,
    '#description' => t('Lossless derivative are of higher quality, but will adversely affect browser performance.'),
    '#type' => 'checkbox',
  );


  $form['islandora_use_kakadu'] = array(
    '#prefix' => '<div id="islandora-use-kakadu-wrapper">',
    '#suffix' => '</div>',
    '#title' => t("Use Kakadu for image compression?"),
    '#disabled' => !$im_jp2000,
    '#default_value' => $kakadu_checked || !$im_jp2000,
    '#description' => t("!kakadu offers faster derivative creation than the standard ImageMagick package. %magick_info", array(
      '!kakadu' => l(t('kakadu'), 'http://www.kakadusoftware.com/'),
      '%magick_info' => $im_jp2000?
        t('ImageMagick reports support of JPEG-2000s.'):
        t('ImageMagick does not report support of JPEG-2000.'),
    )),
    '#type' => 'checkbox',
  );

  $form['islandora_large_image_uncompress_tiff'] = array(
    '#type' => 'checkbox',
    '#title' => t('Uncompress TIFF files prior to creating JP2?'),
    '#description' => t('The version of Kakadu shipped with Djatoka does not ' .
      'support compressed TIFFs; therefore, it is likely desirable to ' .
      'uncompress the TIFF so Kakadu does not encounter an error. This will ' .
      'not change the original TIFF stored in the OBJ datastream. Only ' .
      'disable if you are completely sure!'),
    '#default_value' => variable_get('islandora_large_image_uncompress_tiff', TRUE),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_use_kakadu"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['islandora_kakadu_url'] = array(
    '#prefix' => '<div id="islandora-url">',
    '#suffix' => '</div>',
    '#type' => 'textfield',
    '#title' => t("Path to kakadu"),
    '#default_value' => $kakadu_url,
    '#description' => t('Path to kakadu <br /> !confirmation_message', array('!confirmation_message' => $confirmation_message)),
    '#ajax' => array(
      'callback' => 'islandora_update_kakadu_url_div',
      'wrapper' => 'islandora-url',
      'effect' => 'fade',
      'event' => 'blur',
      'progress' => array('type' => 'throbber'),
    ),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_use_kakadu"]' => array('checked' => TRUE),
      ),
    ),
  );

  module_load_include('inc', 'islandora', 'includes/solution_packs');


  // get viewer table
  $viewer_table = islandora_viewers_form('islandora_large_image_viewers', 'image/jp2');
  // add viewer table
  $form += $viewer_table;

  // actions
  $form['actions'] = array(
    '#type' => 'actions'
  );
  $form['actions']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset to defaults'),
    '#weight' => 1,
    '#submit' => array('islandora_large_image_admin_submit'),
  );

  return system_settings_form($form);
}

/**
 * Form submit
 */
function islandora_large_image_admin_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#id'];
  switch ($op) {
    case 'edit-reset':
      variable_del('islandora_large_image_viewers');
      break;
  }
}

function islandora_update_kakadu_url_div($form, $form_state) {
  return $form['islandora_kakadu_url'];
}
