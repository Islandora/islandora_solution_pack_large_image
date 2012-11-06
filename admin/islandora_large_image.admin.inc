<?php

/**
 * @file
 *
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
  // include solution packs inc file
  module_load_include('inc', 'islandora', 'includes/solution_packs');

  $form = array();
  // get default tab checkbox
  $use_for_default_tab = islandora_use_for_default_tab_form('islandora_large_image_use_for_default_tab');
  // add default tab checkbox
  $form += $use_for_default_tab;

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
 * Form validate
 */
function islandora_large_image_admin_validate($form, &$form_state) {

}

/**
 * Form submit
 */
function islandora_large_image_admin_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#id'];
  switch ($op) {
    case 'edit-reset':
      variable_del('islandora_large_image_use_for_default_tab');
      variable_del('islandora_large_image_viewers');
      break;
  }
}
