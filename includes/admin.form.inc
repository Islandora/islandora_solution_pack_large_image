<?php

/**
 * @file
 * Handles the display/submission of the admin settings form for this module.
 */

/**
 * Defines the admin settings form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_large_image_admin(array $form, array &$form_state) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  module_load_include('inc', 'islandora_large_image', 'includes/utilities');
  $get_default_value = function($name, $default) use(&$form_state) {
    return isset($form_state['values'][$name]) ? $form_state['values'][$name] : variable_get($name, $default);
  };
  $imagemagick_supports_jp2000 = islandora_large_image_check_imagemagick_for_jpeg2000();
  $kakadu = $get_default_value('islandora_kakadu_url', '/usr/bin/kdu_compress');
  $form = array(
    'islandora_lossless' => array(
      '#type' => 'checkbox',
      '#title' => t("Create Lossless Derivatives"),
      '#default_value' => $get_default_value('islandora_lossless', FALSE),
      '#description' => t('Lossless derivatives are of higher quality but adversely affect browser performance.'),
    ),
    // Defaults to trying to use Kakadu if ImageMagick does not support JP2Ks.
    'islandora_use_kakadu' => array(
      '#type' => 'checkbox',
      '#title' => t("Use Kakadu for image compression"),
      '#disabled' => !$imagemagick_supports_jp2000,
      '#default_value' => $get_default_value('islandora_use_kakadu', !$imagemagick_supports_jp2000) || !$imagemagick_supports_jp2000,
      '#description' => t("!kakadu offers faster derivative creation than the standard ImageMagick package. %magick_info", array(
                        '!kakadu' => l(t('Kakadu'), 'http://www.kakadusoftware.com/'),
                        '%magick_info' => $imagemagick_supports_jp2000 ?
                        t('ImageMagick reports support for JPEG 2000.') :
                        t('ImageMagick does not report support for JPEG 2000.'),
                      )),
    ),
    'islandora_large_image_uncompress_tiff' => array(
      '#type' => 'checkbox',
      '#title' => t('Uncompress TIFF files prior to creating JP2 datastreams'),
      '#description' => t('The version of Kakadu shipped with djatoka does not support compressed TIFFs; therefore, it is likely desirable to uncompress the TIFF so Kakadu does not encounter an error. This will not change the original TIFF stored in the OBJ datastream. Only disable this if you are completely sure!'),
      '#default_value' => $get_default_value('islandora_large_image_uncompress_tiff', TRUE),
      '#states' => array(
        'visible' => array(
          ':input[name="islandora_use_kakadu"]' => array('checked' => TRUE),
        ),
      ),
    ),
    'islandora_kakadu_url' => array(
      '#type' => 'textfield',
      '#title' => t("Path to Kakadu"),
      '#default_value' => $kakadu,
      '#description' => t('Path to the kdu_compress executable.<br/>!msg', array('!msg' => islandora_executable_available_message($kakadu))),
      '#prefix' => '<div id="kakadu-wrapper">',
      '#suffix' => '</div>',
      '#ajax' => array(
        'callback' => 'islandora_update_kakadu_url_div',
        'wrapper' => 'kakadu-wrapper',
        'effect' => 'fade',
        'event' => 'blur',
        'progress' => array('type' => 'throbber'),
      ),
      '#states' => array(
        'visible' => array(
          ':input[name="islandora_use_kakadu"]' => array('checked' => TRUE),
        ),
      ),
    ),
    'derivative_med_size' => array(
      '#type' => 'textfield',
      '#title' => t('Derivative Medium Size'),
      '#default_value' => $get_default_value('derivative_med_size', '600 x 800'),
      '#description' => t('The medium size image derivative is default is <b>600 x 800</b> (maximum width by maximum height), not effect relative ratio. Please use a number space the lowercase letter x space and a number.<br/><span style="color:red">WARNING: This will set the size for all medium sized images generated as a derivative. Regenerate derivatives to apply this setting to existing objects.</span>'),
      '#required' => TRUE,
    ),
    'derivative_tn_size' => array(
      '#type' => 'textfield',
      '#title' => t('Derivative TN Size'),
      '#default_value' => $get_default_value('derivative_tn_size', '200 x 200'),
      '#description' => t('The medium size image derivative is default is <b>600 x 800</b> (maximum width by maximum height), not effect relative ratio. Please use a number space the lowercase letter x space and a number.<br/><span style="color:red">WARNING: This will set the size for all thumbnail images generated as a derivative. Regenerate derivatives to apply this setting to existing objects.</span>'),
      '#required' => TRUE,
    ),
  );
  module_load_include('inc', 'islandora', 'includes/solution_packs');
  $form += islandora_viewers_form('islandora_large_image_viewers', 'image/jp2');
  $form['actions'] = array(
    '#type' => 'actions',
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
 * Form submit.
 */
function islandora_large_image_admin_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#id'];
  switch ($op) {
    case 'edit-reset':
      $variables = array(
        'islandora_large_image_viewers',
        'islandora_use_kakadu',
        'islandora_lossless',
        'islandora_large_image_uncompress_tiff',
        'islandora_kakadu_url',
        'derivative_med_size',
        'derivative_tn_size',
      );
      array_walk($variables, 'variable_del');
      break;
  }
}

/**
 * Function update_kakadu_url_div.
 */
function islandora_update_kakadu_url_div($form, $form_state) {
  return $form['islandora_kakadu_url'];
}

/**
* Function to validate the inputs before saving.
*/
function islandora_large_image_admin_validate($form, &$form_state) {
  $check_size_inputs=['derivative_med_size','derivative_tn_size'];
  $non_number_found=0;

  // Checks to see if the values are exclusively numeric.
  foreach ($check_size_inputs as $derivative_size => $form_state_values) {
    $checking_numbers = explode(" x ", $form_state['values'][$form_state_values]);
    foreach ($checking_numbers as $key => $value) {
        if (!is_numeric($value)){
          $non_number_found = 1;
        }
    }
  }
  if ($non_number_found == 1) {
    form_set_error('colour', t('The Derivative sizes must follow the expected format. #0000 x #0000'));
  }
}
