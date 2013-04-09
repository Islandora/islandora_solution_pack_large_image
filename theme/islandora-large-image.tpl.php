<?php

/**
 * @file
 * This is the template file for the object page for large image
 *
 * Available variables:
 * - $islandora_object: The Islandora object rendered in this template file
 * - $islandora_dublin_core: The DC datastream object
 * - $dc_array: The DC datastream object values as a sanitized array. This
 *   includes label, value and class name.
 * - $islandora_object_label: The sanitized object label.
 * - $parent_collections: An array containing parent collection(s) info.
 *   Includes collection object, label, url and rendered link.
 * - $islandora_thumbnail_img: A rendered thumbnail image.
 * - $islandora_content: A rendered image. By default this is the JPG datastream
 *   which is a medium sized image. Alternatively this could be a rendered
 *   viewer which displays the JP2 datastream image.
 *
 * @see template_preprocess_islandora_large_image()
 * @see theme_islandora_large_image()
 */
?>

<div class="islandora-large-image-object islandora">
  <div class="islandora-large-image-content-wrapper clearfix">
    <?php if ($islandora_content): ?>
      <?php if (isset($image_clip)): ?>
        <?php print $image_clip; ?>
      <?php endif; ?>
      <div class="islandora-large-image-content">
        <?php print $islandora_content; ?>
      </div>
    <?php endif; ?>
  <div class="islandora-large-image-sidebar">
    <?php if (!empty($dc_array['dc:description']['value'])): ?>
      <h2><?php print $dc_array['dc:description']['label']; ?></h2>
      <p><?php print $dc_array['dc:description']['value']; ?></p>
    <?php endif; ?>
    <?php if ($parent_collections): ?>
      <div>
        <h2><?php print t('In collections'); ?></h2>
        <ul>
          <?php foreach ($parent_collections as $collection): ?>
        <li><?php print l($collection->label, "islandora/object/{$collection->id}"); ?></li>
          <?php endforeach; ?>
        </ul>
      </div>
    <?php endif; ?>
  </div>
  </div>
  <fieldset class="collapsible collapsed islandora-large-image-metadata">
  <legend><span class="fieldset-legend"><?php print t('Details'); ?></span></legend>
    <div class="fieldset-wrapper">
      <dl class="islandora-inline-metadata islandora-large-image-fields">
        <?php $row_field = 0; ?>
        <?php foreach($dc_array as $key => $value): ?>
          <dt class="<?php print $value['class']; ?><?php print $row_field == 0 ? ' first' : ''; ?>">
            <?php print $value['label']; ?>
          </dt>
          <dd class="<?php print $value['class']; ?><?php print $row_field == 0 ? ' first' : ''; ?>">
            <?php print $value['value']; ?>
          </dd>
          <?php $row_field++; ?>
        <?php endforeach; ?>
      </dl>
    </div>
  </fieldset>
</div>
