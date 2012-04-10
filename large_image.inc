<?php

/**
 * @file
 * LargeImageProcessor and ShowLargeImagesInFieldSets classes
 */

/**
 * This Class implements the methods defined in the basic image solution content model
 */
class LargeImageProcessor {

  /**
   * Constructor
   */
  function LargeImageProcessor() {
    module_load_include('inc', 'fedora_repository', 'api/ObjectHelper');
  }

  /**
   * Create derivatives
   * @param type $parameterArray
   * @param type $dsid
   * @param type $file
   * @param type $file_ext
   * @return type
   */
  function createDerivatives($parameterArray = NULL, $dsid, $file, $file_ext) {
    $file_suffix = "_$dsid.$file_ext";
    $return_value = TRUE;

    $output = array();
    $_SESSION['fedora_ingest_files']['JP2'] = $file;
    exec('kdu_compress -i "' . $file . '" -o "' . $file . $file_suffix . '" -rate 0.5 Clayers=1 Clevels=7 "Cprecincts={256,256},{256,256},{256,256},{128,128},{128,128},{64,64},{64,64},{32,32},{16,16}" "Corder=RPCL" "ORGgen_plt=yes" "ORGtparts=R" "Cblk={32,32}" Cuse_sop=yes', $output, $returnValue);

    if ($returnValue == '0') {
      $_SESSION['fedora_ingest_files'][$dsid] = $file . $file_suffix;
    }
    else {
      ObjectHelper::warnIfMisconfigured('Kakadu');
    }

    exec("convert -resize 600x800\> \"{$file}\"[0]  \"$file-med.jpg\"", $output, $returnValue);
    if ($returnValue == '0') {
      $_SESSION['fedora_ingest_files']["JPG"] = $file . '-med.jpg';
    }
    else {
      ObjectHelper::warnIfMisconfigured('ImageMagick');
    }
    exec("convert \"{$file}\"[0] -thumbnail x2000 -thumbnail x450  -resize '450x<' -resize 50% -fuzz 1% -trim +repage -gravity center -crop 200x200+0+0 +repage -format jpg -quality 100  \"$file-tn.jpg\"", $output, $returnValue);
    if ($returnValue == '0') {
      $_SESSION['fedora_ingest_files']["TN"] = $file . '-tn.jpg';
    }
    else {
      ObjectHelper::warnIfMisconfigured('ImageMagick');
    }

    $path_info = pathinfo($file);
    $newfile = str_replace('.' . $path_info['extension'], '_EXIF.xml', $file);
    $returnValue = TRUE;
    $output = array();
    exec('exiftool -X ' . escapeshellarg($file) . '', $output, $returnValue);
    if ($returnValue == 0) {
      file_put_contents($newfile, implode("\n", $output));
      $_SESSION['fedora_ingest_files']["EXIF"] = $newfile;
    }
    if ($returnValue == '0') {
      return TRUE;
    }
    else {

      return $returnValue;
    }
  }

}

/**
 * Show Large Image In FieldSets
 */
class ShowLargeImagesInFieldSets {

  private $pid = NULL;

  /**
   * Constructor
   * @param type $pid
   */
  function ShowLargeImagesInFieldSets($pid) {
    $this->pid = $pid;
  }

  /**
   * Show JPEG 2000
   * @param type $collapsed
   * @return type
   */
  function showJP2($collapsed = FALSE) {
    $viewer_url = variable_get('fedora_base_url', '') . '/get/' . $this->pid . '/islandora:viewerSdef/getViewer';
    $html = '<iframe src="' . $viewer_url . '" frameborder="0" style="width: 100%; height: 400px;">Errors: unable to load viewer</iframe>';
    $fieldset = array(
      '#title' => t('Viewer'),
      '#collapsible' => TRUE,
      '#collapsed' => $collapsed,
      '#value' => $html
    );
    return theme('fieldset', $fieldset);
  }

  /**
   * Show JPEG
   * @global type $base_url
   * @global type $user
   * @return string
   */
  function showJPG() {
    module_load_include('inc', 'fedora_repository', 'plugins/tagging_form');
    module_load_include('inc', 'fedora_repository', 'plugins/ShowStreamsInFieldSets');
    global $base_url;
    global $user;

    $tabset = array();

    $qs = '';
    if ($user->uid != 0) {
      $qs = '?uid=' . base64_encode($user->name . ':' . $user->pass);
    }

    $viewer_url = variable_get('fedora_base_url', 'http://localhost:8080/fedora') . '/get/' . $this->pid . '/islandora:viewerSdef/getViewer' . $qs;
    $html = '<iframe src="' . $viewer_url . '" scrolling="no" frameborder="0" style="width: 100%; height: 800px;">Errors: unable to load viewer</iframe>';

    drupal_add_css(path_to_theme() . '/header-viewer.css', 'theme');

    $tabset['second_tab'] = array(
      '#type' => 'tabpage',
      '#title' => t('Full-size'),
      '#content' => $html);

    $tabset['first_tab'] = array(
      // #type and #title are the minimum requirements.
      '#type' => 'tabpage',
      '#title' => t('View'),
      // This will be the content of the tab.
      '#content' => '<img src="' . $base_url .
      '/fedora/imageapi/' . $this->pid . '/JPG/JPG.jpg' . '" />' . '<p>' . drupal_get_form('fedora_repository_image_tagging_form', $this->pid) . '</p>',
    );
    return $tabset;
  }

}
