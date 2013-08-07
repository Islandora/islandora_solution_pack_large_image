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
    $command_output = NULL;

    $e = exec(escapeshellcmd(variable_get('imagemagick_convert', 'convert')) . " -list format", $command_output);
    $filtered = preg_filter("/.*JP2.*JPEG-2000\s+File\s+Format\s+Syntax/", "\\0", $command_output);
    $supports = FALSE;
    foreach ($filtered as $f) {
      $s = array();
      if (preg_match_all('/([r-][w-].)/', $f, $s)) {
        if (strpos($s[1][0], 'w') !== FALSE) {
          $supports = TRUE;
          break;
        }
      }
    }
  }

  return $supports;
}
