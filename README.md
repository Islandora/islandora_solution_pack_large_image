BUILD STATUS
------------
Current build status:
[![Build Status](https://travis-ci.org/Islandora/islandora_solution_pack_large_image.png?branch=7.x)](https://travis-ci.org/Islandora/islandora_solution_pack_large_image)

CI Server:
http://jenkins.discoverygarden.ca

Large Image Solution Pack
=========================

The large image solution pack loads all required Fedora objects, and creates an empty collection object
to accept tiff's and create derivatives.

The large image solution pack is dependent on the imagemagick module. Make sure
you have imagemagick enabled as the default image processor

To successfully create derivative data streams ImageMagick (TN & JPG) and Kakadu (JP2) need to be installed on the server.

To use ImageMagick you must also configure the image-tool kit on drupal use ImageMagick rather than GD otherwise (TN & JPG) data streams will not be generated.
Goto "admin/config/media/image-toolkit" and select "ImageMagick"

To use Kakadu, make sure that kdu_compress and kdu_expand are avaliable in the terminal. Often users will create symbolic links from /usr/local/bin/kdu_compress to their installation of Kakadu that comes included with Adore-Djatoka (http://sourceforge.net/apps/mediawiki/djatoka/index.php?title=Installation). Make sure that the required dynamic libriraries that come with Kakadu are excessible to the
programs kdu_compress and kdu_expand. If they are not present attempting to run either command from the terminal will inform you it's libraries are missing. You can also use a symbolic link from /usr/local/lib to include these libraries, remember to restart the terminal so your changes take affect.

Make sure the php settings allow for enough memory and upload size.
(upload_max_filesize, post_max_size and memory_limit)

Suggested viewers:
- islandora_openseadragon
