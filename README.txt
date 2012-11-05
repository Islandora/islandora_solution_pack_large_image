Large Image Solution Pack for Islandora
Load all required Fedora objects, and creates empty collection object
to accept tiff's and create deriviatives.

Make sure your php settings allow for enough memory and upload size.
(upload_max_filesize, post_max_size and memory_limit)

The large image solution pack is dependent on the imagemagick module. Make sure
you have imagemagick enabled as the default image processor

To succesfully create derivative datastreams ImageMagick (TN & JPG) and Kakadu (JP2) need to be installed on the server.


TODO:
=====

high

- get drupal_set_title out of the template file !!
- better error handling. -> imageMagick and kakadu error messages + watchdog.
- use 'islandora_large_image' and append '_viewers' instead of 'islandora_large_image_viewers' ?
- images in preprocess functions need alt attributes !
- evaluate current implementation
- check for code reusablity in derivatives
-- remove variables in uninstall hook
- documentation
- add islandora_large_image.api.php
- fix DS labels?
- make sure that file objects and loose files are removed.

medium

- write interface similar to imagemagick in drupal to configure kakadu.

low