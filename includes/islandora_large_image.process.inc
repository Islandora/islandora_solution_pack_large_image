<?php

/**
 * @file
 *
 * This file contains all the functions for image manipulation used in the large image solution pack.
 */

/**
 * Creates the derivatives for this content model type
 *
 * @param FedoraObject $object
 *   The object to add derivatives to.
 *
 * @return boolean
 *   TRUE if all derivatives were created successfully, FALSE otherwise.
 */
function islandora_large_image_create_all_derivatives(FedoraObject $object) {
  if (!isset($object['OBJ'])) {
    drupal_set_message(t('Could not create image derivatives for %s.  No image file was uploaded.', array('%s' => $object->id)), 'error');
    return FALSE;
  }
  $base_name = str_replace(':', '-', $object->id);
  $uploaded_file = islandora_large_image_get_uploaded_file($object, $base_name);
  if ($uploaded_file) {
    $jp2 = islandora_large_image_create_JP2_derivative($object, $uploaded_file, $base_name);
    $jpg = islandora_large_image_create_JPG_derivative($object, $uploaded_file, $base_name);
    $tn = islandora_large_image_create_TN_derivative($object, $uploaded_file, $base_name);
    return $jp2 && $jpg && $tn && file_unmanaged_delete($uploaded_file);
  }
  return FALSE;
}

/**
 * Fetches the upload image file from Fedora, and saves it to a temp location.
 *
 * @param FedoraObject $object
 *  The object to fetch the uploaded image file from.
 * @param string $base_name
 *  The base name to use in creating temp files.
 *
 * @return string
 *   The file path to the temp file if successful, FALSE otherwise.
 */
function islandora_large_image_get_uploaded_file(FedoraObject $object, $base_name) {
  $mime_detector = new MimeDetect();
  $ext = $mime_detector->getExtension($object['OBJ']->mimeType);
  return file_unmanaged_save_data($object['OBJ']->content, "temporary://{$base_name}_OBJ.{$ext}");
}

/**
 * Creates the JP2 derived datatstream from the uploaded image file.
 *
 * @param FedoraObject $object
 *   The object to add the derived datastream to.
 * @param string $uploaded_file
 *   The file path to the uploaded image file to derive from.
 * @param string $base_name
 *   The base name to use in creating temp files.
 *
 * @return boolean
 *   TRUE if successful, FALSE otherwise.
 */
function islandora_large_image_create_JP2_derivative(FedoraObject $object, $uploaded_file, $base_name) {
  // create JP2 with kakadu
  $kakadu = variable_get('islandora_use_kakadu', FALSE);
  $mime = strtolower($object['OBJ']->mimetype);
  if ($mime == 'image/jpeg' || $mime == 'image/jpg') {
    $kakadu = FALSE;
  }

  if ($kakadu) {
    $derivative_file = islandora_large_image_kdu_compress($uploaded_file, "temporary://{$base_name}_JP2.jp2"); // Create JP2
    if (!$derivative_file) {
      drupal_set_message(t("Kakadu failed.  Trying ImageMagick"));
      $kakadu = FALSE; //forces retry with IMageMagick if Kakadu has failed - allows compressed tiffs if unsupported by Kakadu version
    }
  }
  if (!$kakadu) {
    $derivative_file = islandora_large_image_imagemagick_convert($uploaded_file, "temporary://{$base_name}_JP2.jp2", islandora_large_image_get_args());
  }
  if ($derivative_file === FALSE) {
    drupal_set_message(t('Failed to create JP2 derivative'), 'error');
    return FALSE;
  }
  islandora_large_image_add_datastream($object, 'JP2', $derivative_file, 'image/jp2', t('JPEG 2000'));
  return file_unmanaged_delete($derivative_file);
}

/**
 * Creates the JPG derived datatstream from the uploaded image file.
 *
 * @param FedoraObject $object
 *   The object to add the derived datastream to.
 * @param string $uploaded_file
 *   The file path to the uploaded image file to derive from.
 * @param string $base_name
 *   The base name to use in creating temp files.
 *
 * @return boolean
 *   TRUE if successful, FALSE otherwise.
 */
function islandora_large_image_create_JPG_derivative(FedoraObject $object, $uploaded_file, $base_name) {
  $args = array();
  $args[] = '-resize ' . escapeshellarg("600 x 800");
  $args[] = '-quality ' . escapeshellarg(variable_get('imagemagick_quality', 75));
  $derivative_file = islandora_large_image_imagemagick_convert($uploaded_file, "temporary://{$base_name}_JPG.jpg", $args);
  if ($derivative_file === FALSE) {
    drupal_set_message(t('Failed to create JPG derivative'), 'error');
    return FALSE;
  }
  islandora_large_image_add_datastream($object, 'JPG', $derivative_file, 'image/jpeg', t('Medium sized JPEG'));
  return file_unmanaged_delete($derivative_file);
}

/**
 * Creates the TN derived datatstream from the uploaded image file.
 *
 * @param FedoraObject $object
 *   The object to add the derived datastream to.
 * @param string $uploaded_file
 *   The file path to the uploaded image file to derive from.
 * @param string $base_name
 *   The base name to use in creating temp files.
 *
 * @return boolean
 *   TRUE if successful, FALSE otherwise.
 */
function islandora_large_image_create_TN_derivative(FedoraObject $object, $uploaded_file, $base_name) {
  $args = array();
  $args[] = '-quality ' . escapeshellarg(variable_get('imagemagick_quality', 75));
  $args[] = '-resize ' . escapeshellarg("200 x 200");
  $derivative_file = islandora_large_image_imagemagick_convert($uploaded_file, "temporary://{$base_name}_TN.jpg", $args);
  if ($derivative_file === FALSE) {
    drupal_set_message(t('Failed to create TN derivative'), 'error');
    return FALSE;
  }
  islandora_large_image_add_datastream($object, 'TN', $derivative_file, 'image/jpeg', t('Thumbnail'));
  return file_unmanaged_delete($derivative_file);
}

/**
 * Calls kdu_compress with the given arguments, or defaults if none are provided.
 *
 * @param string $src
 *   The source file to pass to kdu_compress
 * @param string $dest
 *   The destination file which kdu_compress will generate.
 * @param string $args
 *   Any additional arguments for kdu_compress, if not provided reasonable defaults will be used.
 *
 * @return
 *   Returns the newly generated file uri or FALSE if the conversion failed
 */
function islandora_large_image_kdu_compress($src, $dest, $args = NULL) {
  $args = isset($args) ? $args : '-rate 0.5 Clayers=1 Clevels=7 "Cprecincts={256,256},{256,256},{256,256},{128,128},{128,128},{64,64},{64,64},{32,32},{16,16}" "Corder=RPCL" "ORGgen_plt=yes" "ORGtparts=R" "Cblk={32,32}" Cuse_sop=yes'; // Default args.
  $src = drupal_realpath($src);
  $dest = drupal_realpath($dest);
  $ret = -1;
  $output = array();
  $command = "kdu_compress -i \"$src\" -o \"$dest\" $args 2>&1";
  exec($command, $output, $ret);
  if ($ret != 0) {
    $message = 'kdu_compress failed to create derivative<br/>Error: @ret<br/>Command: @command <br/>Output !output';
    $variables = array('@ret' => $ret, '@command' => $command, '!output' => implode('<br/>', $output));
    watchdog('islandora_large_image', $message, $variables, WATCHDOG_ERROR);
    return FALSE;
  }
  return $dest;
}

/**
 * Calls imagemagick's convert command with the given arguments.
 *
 * @param string $src
 *   The source file uri to convert from.
 * @param string $dest
 *   The destination file uri to convert into.
 * @param int $width
 *   The source file to convert from.
 * @param int $height
 *   The destination file to convert into.
 *
 * @return string
 *   The destination file path if successful otherwise FALSE.
 */
function islandora_large_image_imagemagick_convert($src, $dest, $args) {
  $src = drupal_realpath($src) . '[0]';
  $dest = drupal_realpath($dest);
  $context = array(
    'source' => $src,
    'destination' => $dest,
  );
  drupal_alter('imagemagick_arguments', $args, $context);
  /**
   * To make use of ImageMagick 6's parenthetical command grouping we need to make the $source image the first parameter and $dest the last.
   * See http://www.imagemagick.org/Usage/basics/#cmdline
   */
  $command = escapeshellarg($src) . ' ' . implode(' ', $args) . ' ' . escapeshellarg("jpg:$dest");
  $output = '';
  $ret = -1;
  if (_imagemagick_convert_exec($command, $output, $ret) !== TRUE) {
    $message = 'imagemagick convert failed to create derivative<br/>Error: @ret<br/>Command: @command<br/>Output: @output';
    $variables = array('@ret' => $ret, '@command' => $command, '@output' => $output);
    watchdog('islandora_large_image', $message, $variables, WATCHDOG_ERROR);
    return FALSE;
  }
  return $dest;
}

/**
 * Adds a datastream to an object.
 *
 * @param object $object
 *   The object to add a datastream to.
 * @param string $dsid
 *   The datastream ID to be added.
 * @param string $file
 *   The URI of the file to be added as the datastream content.
 * @param string $mimetype
 *   The mimetype of the file.
 * @param string $label
 *   The label for the datastream.
 *
 * @TODO: could this be a generic function? eg: islandora_add_datastream($object, $dsid, $file). ?
 */
function islandora_large_image_add_datastream($object, $dsid, $file, $mimetype, $label) {
  $ds = $object->constructDatastream($dsid, 'M');
  $ds->label = $label;
  $ds->mimeType = $mimetype;
  $ds->setContentFromFile($file);
  try {
    $object->ingestDatastream($ds);
  } catch (exception $e) {
    drupal_set_message(t('@message', array('@message' => check_plain($e->getMessage()))), 'error');
    return FALSE;
  }
  return TRUE;
}

/**
 * Return preconfigured paramaters for ImageMagick JP2 creation
 * @return array
 *
 */

function islandora_large_image_get_args() {

  if (variable_get('islandora_lossless', FALSE)) {
    $args[] = " -define numrlvls=6";
    $args[] = " -define jp2:tilewidth=1024";
    $args[] = " -define jp2:tileheight=1024";
    $args[] = " -define jp2:rate=1.0";
    $args[] = " -define jp2:lazy";
    $args[] = " -define jp2:prg=rlcp";
    $args[] = " -define jp2:ilyrrates='0.015625,0.01858,0.0221,0.025,0.03125,0.03716,0.04419,0.05,0.0625, 0.075,0.088,0.1,0.125,0.15,0.18,0.21,0.25,0.3,0.35,0.4,0.5,0.6,0.7,0.84'";
    $args[] = " -define jp2:mode=int";
  }
  else {
    $args[] = "-define numrlvls=7";
    $args[] = "-define jp2:tilewidth=1024";
    $args[] = "-define jp2:tileheight=1024";
    $args[] = "-define jp2:rate=0.02348";
    $args[] = "-define jp2:prg=rpcl";
    $args[] = "-define jp2:mode=int";
    $args[] = "-define jp2:prcwidth=16383";
    $args[] = "-define jp2:prcheight=16383";
    $args[] = "-define jp2:cblkwidth=64";
    $args[] = "-define jp2:cblkheight=64";
    $args[] = "-define jp2:sop";
  }
  return $args;
}
