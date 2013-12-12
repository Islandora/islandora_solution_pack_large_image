<?php
/**
 * @file
 * Utility functions.
 */

/**
 * Determine if ImageMagick appears to support JPEG-2000.
 *
 * @return bool
 *   If JPEG-2000 appears in the list of supported formats.
 */
function islandora_large_image_check_imagemagick_for_jpeg2000() {
  $supports =& drupal_static(__FUNCTION__);

  if ($supports === NULL) {
    $command = variable_get('imagemagick_convert', 'convert');
    if (variable_get('imagemagick_gm', 0)) {
      $command .= ' convert';
    }
    $command .= " -list format";
    $command_output = NULL;
    $e = exec($command, $command_output);
    $filtered = preg_filter("/.*JP2.*rw.*JPEG-2000/", "\\0", $command_output);
    $supports = !empty($filtered);
  }
  return $supports;
}
