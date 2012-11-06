Large Image Solution Pack
=========================

The large image solution pack loads all required Fedora objects, and creates an empty collection object
to accept tiff's and create deriviatives.

The large image solution pack is dependent on the imagemagick module. Make sure
you have imagemagick enabled as the default image processor

To succesfully create derivative datastreams ImageMagick (TN & JPG) and Kakadu (JP2) need to be installed on the server.

Make sure the php settings allow for enough memory and upload size.
(upload_max_filesize, post_max_size and memory_limit)

Suggested viewers:
- islandora_openseadragon