<?php



/**
 * @file
 * 
 */
module_load_include('inc', 'fedora_repository', 'plugins/FormBuilder');

/**
 * implements methods from content model ingest form xml
 * builds a dc metadata form
 */

class LargeImageFormBuilder extends FormBuilder {

  /**
   * Constructor
   */
  function LargeImageFormBuilder() {
    module_load_include('inc', 'fedora_repository', 'plugins/FormBuilder');
    drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
  }


  function createFedoraDataStreams($form_values, &$dom, &$rootElement) {
    module_load_include('inc', 'fedora_repository', 'MimeClass');
    global $base_url;
    $mimetype = new MimeClass();
    $server = NULL;

    if (!empty($_SESSION['fedora_ingest_files'])) {
      foreach ($_SESSION['fedora_ingest_files'] as $dsid => $createdFile) {
        $createdFile = strstr($createdFile, $file);
        $dformat = $mimetype->getType($createdFile);
        $fileUrl = $base_url . '/' . drupal_urlencode($createdFile);
        $beginIndex = strrpos($fileUrl, '/');
        $dtitle = substr($fileUrl, $beginIndex + 1);
        $dtitle = urldecode($dtitle);
        //				$dtitle =  substr($dtitle, 0, strpos($dtitle, "."));
        $dtitle = $dtitle;
        $ds1 = $dom->createElement("foxml:datastream");
        $ds1->setAttribute("ID", "$dsid");
        $ds1->setAttribute("STATE", "A");
        $ds1->setAttribute("CONTROL_GROUP", "M");
        $ds1v = $dom->createElement("foxml:datastreamVersion");
        $ds1v->setAttribute("ID", "$dsid.0");
        $ds1v->setAttribute("MIMETYPE", "$dformat");
        $ds1v->setAttribute("LABEL", "$dtitle");
        $ds1content = $dom->createElement('foxml:contentLocation');
        $ds1content->setAttribute("REF", "$fileUrl");
        $ds1content->setAttribute("TYPE", "URL");
        $ds1->appendChild($ds1v);
        $ds1v->appendChild($ds1content);
        $rootElement->appendChild($ds1);
      }
    }
  }

}

