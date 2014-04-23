# Large Image Solution Pack [![Build Status](https://travis-ci.org/Islandora/islandora_solution_pack_large_image.png?branch=7.x)](https://travis-ci.org/Islandora/islandora_solution_pack_large_image)

## Introduction

The large image solution pack loads all required Fedora objects, and creates an empty collection object to accept tiff's and create derivatives.

## Requirements

This module requires teh following modules/libraries:

* [Islandora](https://github.com/islandora/islandora)
* [Tuque](https://github.com/islandora/tuque)
* [ImageMagick](https://drupal.org/project/imagemagick)
* Kakadu (bundled with Djatoka)

*To successfully create derivative data streams ImageMagick (TN & JPG) and Kakadu (JP2) need to be installed on the server.*

## Installation

Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information.

## Configuration

Configure the image-tool kit to use ImageMagick rather than GD in Administration > Configuration > Media > Image Toolkit (admin/config/media/image-toolkit). If GD is selected, TN and JPG datastreams will not be generated.

![Configuration](http://i.imgur.com/O3sQPeO.png)


Select configuration options and viewer in Administration > Islandora > Large Image Collection (admin/islandora/large_image).

To use Kakadu, make sure that `kdu_compress` and `kdu_expand` are avaliable to the Apache user. Often users will create symbolic links from `/usr/local/bin/kdu_compress` to their installation of Kakadu that comes bundled with [Adore-Djatoka](http://sourceforge.net/apps/mediawiki/djatoka/index.php?title=Installation). Make sure that the required dynamic libriraries that come with Kakadu are excessible to `kdu_compress` and `kdu_expand`. If they are not present, attempting to run either command from the terminal will inform you it's libraries are missing. You can also use a symbolic link from `/usr/local/lib` to include these libraries, remember to restart the terminal so your changes take affect. Also, make sure the php settings allow for enough memory and upload size: upload_max_filesize, post_max_size and memory_limit.

![Configuration](http://i.imgur.com/bS5ph4A.png)

## Troubleshooting/Issues

Having problems or solved a problem? Check out the Islandora google groups for a solution.

* [Islandora Group](https://groups.google.com/forum/?hl=en&fromgroups#!forum/islandora)
* [Islandora Dev Group](https://groups.google.com/forum/?hl=en&fromgroups#!forum/islandora-dev)

## Maintainers/Sponsors
Current maintainers:

* [Nick Ruest](https://github.com/ruebot)

## Development

If you would like to contribute to this module, please check out our helpful [Documentation for Developers](https://github.com/Islandora/islandora/wiki#wiki-documentation-for-developers) info, as well as our [Developers](http://islandora.ca/developers) section on the [Islandora.ca](http://islandora.ca) site.

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
