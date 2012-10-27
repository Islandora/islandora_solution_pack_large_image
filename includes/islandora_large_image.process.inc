<?php
/**
 * @file
 * This file contains all the functions for image manipulation used in the
 * large image solution pack.
 */

/**
 * creates the derivatives for this content model type
 * @param object $object
 *   tuque FedoraObject
 * @return string
 */
function islandora_large_image_create_all_derivatives($object) {

  // @TODO: this mimetype stuff is repeated in every solution pack. maybe make generic function?
  module_load_include('inc', 'islandora', 'includes/mime.detect');
  $mime_class = new MimeDetect();
  
  // OBJ is the original image
  if (!isset($object['OBJ'])) {
    drupal_set_message(t('Could not create image derivatives for %s.  No image file was uploaded.', array('%s' => $object->id)), 'error');
    return "";
  }
  // find extention based on mimeType
  $ext = $mime_class->getExtension($object['OBJ']->mimeType);
  $file_name = str_replace(':', '-', $object->id);
  // the original file as a temp file
  $original_file = file_save_data($object['OBJ']->content, 'temporary://' . $file_name . 'OBJ.' . $ext);
  
  // convert from .tif to .jpeg2000
  $jp2_file = islandora_large_image_create_derivative_kakadu($original_file, 'JP2', 'jp2');
  if ($jp2_file) {
    islandora_large_image_add_datastream($object, 'JP2', $jp2_file);
  }
  
  // create thumbnail image and add as datastream
  islandora_large_image_create_derivative_imagemagick($jp2_file, $object, 'TN', 200, 200); // derive from jp2 image
  // create medium size image and add as datastream
  islandora_large_image_create_derivative_imagemagick($jp2_file, $object, 'MEDIUM', 600, 800); // note: D6 does 500x700
  // @TODO: create EXIF?  

  
  
  
  
  // remove temp file // @TODO: remove temp jp2 file too?
  file_delete($original_file);
}


/**
 *
 * @param object $file
 *   stdclass
 *
 * @TODO: could this be a generic function? eg: islandora_create_derivative($file, $width, $height). ?
 */
function islandora_large_image_create_derivative($file, $width, $height) {
  // get file path
  $real_path = drupal_realpath($file->uri);
  // load file as image and scale
  $image = image_load($real_path);
  if (!empty($image)) {
    $scale = image_scale($image, $width, $height, TRUE);
    if ($scale) {
      return image_save($image);
    }
  }
  return FALSE;
}

/**
 *
 * @param object $file
 *   stdclass
 *
 * @TODO: make reusable?
 */
function islandora_large_image_create_derivative_imagemagick($original, $object, $dsid, $width, $height) {

  $source = drupal_realpath($original->uri) . '[0]';
  $temp = drupal_tempnam('temporary://', $dsid);
  $dest = drupal_realpath($temp);

  $args['quality'] = '-quality ' . escapeshellarg(variable_get('imagemagick_quality', 75));
  $args['previewsize'] = '-resize ' . escapeshellarg("{$width}x{$height}");

  $context = array(
    'source' => $source,
    'destination' => $dest,
  );

  drupal_alter('imagemagick_arguments', $args, $context);
  // To make use of ImageMagick 6's parenthetical command grouping we need to make
  // the $source image the first parameter and $dest the last.
  // See http://www.imagemagick.org/Usage/basics/#cmdline
  $command = escapeshellarg($source) . ' ' . implode(' ', $args) . ' ' . escapeshellarg("jpg:$dest");

  if (_imagemagick_convert_exec($command, $output, $error) !== TRUE) {
    return FALSE;
  }

  islandora_large_image_add_datastream($object, $dsid, $dest);
  file_unmanaged_delete($dest);
}

/**
 * Creates a derivative with Kakadu.
 */
function islandora_large_image_create_derivative_kakadu($file, $dsid, $file_ext) {
  // set variables
  $file_suffix = "_$dsid.$file_ext";
  $return_value = TRUE;
  $output = array();

  // .tif to .jp2 file
  exec('kdu_compress -i "' . $file . '" -o "' . $file . $file_suffix . '" -rate 0.5 Clayers=1 Clevels=7 "Cprecincts={256,256},{256,256},{256,256},{128,128},{128,128},{64,64},{64,64},{32,32},{16,16}" "Corder=RPCL" "ORGgen_plt=yes" "ORGtparts=R" "Cblk={32,32}" Cuse_sop=yes', $output, $return_value);
  // if it returns '0' the command is valid?
  if ($return_value == '0') {
    return $file . $file_suffix;
  }
  else {
    return FALSE;
  }
}

/**
 * adds a datastream and deletes the tmp file from local file system
 * @param object $object
 * @param string $dsid
 * @param object $file
 *
 * @TODO: could this be a generic function? eg: islandora_add_datastream($object, $dsid, $file). ?
 */
function islandora_large_image_add_datastream($object, $dsid, $file) {
  try {
    $ds = $object->constructDatastream($dsid, 'M');
    $ds->label = $dsid;
    $ds->mimeType = $object['OBJ']->mimeType;
    $ds->setContentFromFile(drupal_realpath($file->uri));
    $object->ingestDatastream($ds);
    file_delete($file);
  } catch (exception $e) {
    drupal_set_message(t('@message', array('@message' => check_plain($e->getMessage()))), 'error');
  }
}






