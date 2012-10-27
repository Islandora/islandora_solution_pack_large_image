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
function islandora_large_image_admin() {
  // include css
  drupal_add_css(drupal_get_path('module', 'islandora_large_image') . '/css/islandora_large_image.admin.css');

  $form = array();
  $form['islandora_large_image_use_for_default_tab'] = array(
    '#type' => 'checkbox',
    '#title' => t('Provide the view for the default tab'),
    '#default_value' => variable_get('islandora_large_image_use_for_default_tab', TRUE),
    '#description' => t('Should this module provide the view for the default view tab.  If you are seeing unexpected content
      on a view tab you may have multiple modules configured to provide content for the default tab.'),
    '#weight' => -10
  );

  return system_settings_form($form);
}

