<?php

/**
 * @file
 * This file contains all admin functions
 */

/**
 * Admin form definition.
 *
 * @TODO: this seems to be used in every solution pack. Move to islandora core?
 */
function islandora_large_image_admin($form, &$form_state) {
  // include css
  drupal_add_css(drupal_get_path('module', 'islandora_large_image') . '/css/islandora_large_image.admin.css');

  $form = array();
  $form['islandora_large_image_use_for_default_tab'] = array(
    '#type' => 'checkbox',
    '#title' => t('Provide the view for the default tab'),
    '#default_value' => variable_get('islandora_large_image_use_for_default_tab', 1),
    '#description' => t('Should this module provide the view for the default view tab.  If you are seeing unexpected content
      on a view tab you may have multiple modules configured to provide content for the default tab.'),
    '#weight' => -10
  );

  // get viewers
  $viewers = _islandora_large_image_get_viewers();
  // get viewer settings
  $viewers_config = variable_get('islandora_large_image_viewers', array());
  // viewer
  $form['viewers'] = array(
    '#type' => 'fieldset',
    '#title' => t('Viewers'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  if (!empty($viewers)) {
    // viewers table
    $form['viewers']['islandora_large_image_viewers'] = array(
      '#type' => 'item',
      '#title' => t('Select a viewer'),
      '#description' => t('Preferred viewer to display the large image. These may be provided by third-party modules. Fill out a datastream ID to pass to the viewer.'),
      '#tree' => TRUE, // this attribute is important to return the submitted values in a deeper nested arrays in
      '#theme' => 'islandora_large_image_viewers_table',
    );

    // table loop
    foreach ($viewers as $name => $profile) {
  
      $options[$name] = '';
  
      // machine name
      $form['viewers']['islandora_large_image_viewers']['name'][$name] = array(
        '#type' => 'hidden',
        '#value' => $name,
      );
      // label
      $form['viewers']['islandora_large_image_viewers']['label'][$name] = array(
        '#type' => 'item',
        '#markup' => $profile['label'],
      );
      // description
      $form['viewers']['islandora_large_image_viewers']['description'][$name] = array(
        '#type' => 'item',
        '#markup' => $profile['description'],
      );
      // DSID
      if ($name != 'none') {
        $form['viewers']['islandora_large_image_viewers']['dsid'][$name] = array(
          '#type' => 'textfield',
          '#size' => 15,
          '#default_value' => !empty($viewers_config) ? $viewers_config['dsid'][$name] : (isset($profile['default_dsid']) ? $profile['default_dsid'] : ''),
        );  
      }
      // configuration url
      $form['viewers']['islandora_large_image_viewers']['configuration'][$name] = array(
        '#type' => 'item',
        '#markup' => (isset($profile['configuration']) AND $profile['configuration'] != '') ? l(t('configure'), $profile['configuration']) : '',
      );
    }
    // radios
    $form['viewers']['islandora_large_image_viewers']['default'] = array(
      '#type' => 'radios',
      '#options' => isset($options) ? $options : array(),
      '#default_value' => !empty($viewers_config) ? $viewers_config['default'] : 'none',
    );
  }
  else {
    // No viewers found
    $form['viewers']['islandora_large_image_no_viewers'] = array(
      '#markup' => t('No viewers detected.'),
    );
    // remove viewers variable
    variable_del('islandora_large_image_viewers');
  }
  
  
  // actions
  $form['actions'] = array(
    '#type' => 'actions'
  );
  $form['actions']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset to defaults'),
    '#weight' => 1,
    '#submit' => array('_islandora_large_image_admin_submit'),
  );

  return system_settings_form($form);
}

/**
 * Form validate
 */
function islandora_large_image_admin_validate($form, &$form_state) {

}

/**
 * Form submit
 */
function _islandora_large_image_admin_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#id'];
  switch ($op) {
    case 'edit-reset':
      variable_del('islandora_large_image_use_for_default_tab');
      variable_del('islandora_large_image_viewers');
    break;
  }
}

/**
 * Theme function for the admin primary display table
 *
 * @param type $variables
 *   render element: $form
 *   Contains the form array
 * @return
 *   rendered form element
 *
 *  @see islandora_large_image_admin()
 */
function theme_islandora_large_image_viewers_table($variables) {
  // set form
  $form = $variables['form'];
  $rows = array();

  foreach ($form['name'] as $key => $element) {
    // Do not take form control structures.
    if (is_array($element) && element_child($key)) {
      // set rows
      $row = array();
      $row[] = array('data' => drupal_render($form['default'][$key]));
      $row[] = array('data' => drupal_render($form['label'][$key]));
      $row[] = array('data' => drupal_render($form['description'][$key]));
      $row[] = array('data' => drupal_render($form['dsid'][$key]));
      $row[] = array('data' => drupal_render($form['configuration'][$key]));

      // add to rows
      $rows[] = array('data' => $row);
    }
  }

  // Individual table headers.
  // default | label | description | datastream id | configuration
  $header = array();
  $header[] = array('data' => t('Default'));
  $header[] = array('data' => t('Label'));
  $header[] = array('data' => t('Description'));
  $header[] = array('data' => t('Datastream ID'));  
  $header[] = array('data' => t('Configuration'));

  // render form
  $output = '';
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'islandora-large-image-viewers-table')));
  $output .= drupal_render_children($form);

  return $output;
}

/**
 * Helper function to return viewers. Adds a default option when at least one viewer plugin is found
 */
function _islandora_large_image_get_viewers() {
  // get all viewer profiles
  $viewers = module_invoke_all('islandora_large_image_get_viewer');
  // if there's at least one viewer, add the default as an option too.
  if (!empty($viewers)) {
    // define default viewer
    $default_viewer = array();
    $default_viewer['none'] = array(
      'label' => t('None'),
      'description' => t('Don\'t use a viewer. Defaults to medium sized image.'),
    );
    // merge to viewers array
    $viewers = array_merge_recursive($default_viewer, $viewers);
  }
  return $viewers;
}
