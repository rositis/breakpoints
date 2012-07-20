<?php

/**
 * @file
 * Admin settings
 */

function resp_img_admin_suffixes($form, &$form_state, $theme_key) {
  $form = array();

  // Suffixes defined by the theme
  $theme_suffixes = resp_img_suffix_load_all_active_from_theme($theme_key);
  if (!empty($theme_suffixes)) {
    $rows = array();
    foreach ($theme_suffixes as $suffix) {
      $rows[] = array(
        $suffix->name,
        $suffix->breakpoint,
      );
    }
    $form['theme_suffixes'] = array(
      '#tree' => TRUE,
      '#caption' => 'Active suffixes from the theme.',
      '#theme' => 'table',
      '#header' => array(
        t('Name'),
        t('Breakpoint'),
      ),
      '#rows' => $rows,
    );
  }


  $form['suffixes'] = array(
    '#tree' => TRUE,
    '#caption' => 'Suffixes defined by module or user.',
    '#theme' => 'table',
    '#header' => array(
      t('Name'),
      t('Breakpoint'),
      array('data' => t('Operations'), 'colspan' => 2),
    ),
  );

  $suffixes = resp_img_suffix_load_all_userdefined();
  foreach ($suffixes as $suffix) {
    $cell_name = array(
      '#id' => 'name-' . $suffix->name,
      '#type' => 'textfield',
      '#default_value' => $suffix->name,
      '#disabled' => TRUE,
      '#size' => 20,
    );
    $cell_breakpoint = array(
      '#id' => 'breakpoint-' . $suffix->name,
      '#type' => 'textfield',
      '#default_value' => $suffix->breakpoint,
      '#disabled' => $suffix->source === 'theme',
      '#size' => 20,
    );
    $cell_remove = array(
      '#id' => 'remove-' . $suffix->name,
      '#type' => 'submit',
      '#value' => 'Remove',
      '#disabled' => $suffix->source === 'theme',
      '#submit' => array('resp_img_admin_suffixes_remove'),
    );
    $cell_enable = array(
      '#id' => 'enable-' . $suffix->name,
      '#type' => 'submit',
      '#value' => $suffix->status ? 'Disable' : 'Enable',
      '#submit' => array('resp_img_admin_suffixes_enabledisable'),
    );

    $form['suffixes'][] = array(
      'name' => &$cell_name,
      'breakpoint' => &$cell_breakpoint,
      'remove' => &$cell_remove,
      'enable' => &$cell_enable,
    );

    $form['suffixes']['#rows'][] = array(
      array('data' => &$cell_name),
      array('data' => &$cell_breakpoint),
      array('data' => &$cell_remove),
      array('data' => &$cell_enable),
    );

    unset($cell_name);
    unset($cell_breakpoint);
    unset($cell_remove);
    unset($cell_enable);

  }

  // Add empty row
  $cell_name = array(
    '#id' => 'name-' . 'empty',
    '#type' => 'textfield',
    '#default_value' => '',
    '#size' => 20,
  );
  $cell_breakpoint = array(
    '#id' => 'breakpoint-' . 'empty',
    '#type' => 'textfield',
    '#default_value' => '',
    '#size' => 20,
  );

  $form['suffixes'][] = array(
    'name' => &$cell_name,
    'breakpoint' => &$cell_breakpoint,
  );

  $form['suffixes']['#rows'][] = array(
    array('data' => &$cell_name),
    array('data' => &$cell_breakpoint),
    array('data' => '', 'colspan' => 2),
  );

  unset($cell_name);
  unset($cell_breakpoint);
  unset($cell_remove);

  // Submit button
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function resp_img_admin_suffixes_remove($form, &$form_state) {
  $name = substr($form_state['triggering_element']['#id'], strlen('remove-'));
  resp_img_suffix_delete('resp_img.' . $name);
}

function resp_img_admin_suffixes_enabledisable($form, &$form_state) {
  $name = substr($form_state['triggering_element']['#id'], strlen('remove-'));
  resp_img_suffix_toggle_status('resp_img.' . $name);
}

function resp_img_admin_suffixes_validate($form, &$form_state) {
  if (strpos($form_state['triggering_element']['#id'], 'remove') === FALSE) {
    $suffixes = $form_state['values']['suffixes'];
    if (!empty($suffixes)) {
      $unique = array();
      foreach ($suffixes as $key => $suffixdata) {
        if (!empty($suffixdata['name'])) {
          // Name has to be unique.
          if (array_key_exists($suffixdata['name'], $unique)) {
            form_set_error('suffixes][' . $key . '][name', 'Not unique');
          }
          // Breakpoint is required.
          if (empty($suffixdata['breakpoint']) && $suffixdata['breakpoint'] !== '0') {
            form_set_error('suffixes][' . $key . '][breakpoint', 'Required');
          }
          $unique[$suffixdata['name']] = TRUE;
        }
      }
    }
  }
}

function resp_img_admin_suffixes_submit($form, &$form_state) {
  $suffixes = $form_state['values']['suffixes'];
  if (!empty($suffixes)) {
    foreach ($suffixes as $suffixdata) {
      if (!empty($suffixdata['name'])) {
        $suffix = resp_img_suffix_load('resp_img.' . $suffixdata['name']);
        if ($suffix) {
          $suffix->breakpoint = $suffixdata['breakpoint'];
          resp_img_suffix_save($suffix);
        }
        else {
          $suffix = new StdClass();
          $suffix->name = $suffixdata['name'];
          $suffix->suffix = RESP_IMG_SEPARATOR . $suffix->name;
          $suffix->breakpoint = $suffixdata['breakpoint'];
          $suffix->source = 'user';
          $suffix->status = TRUE;
          resp_img_suffix_save($suffix);
        }
      }
    }
  }
}

function resp_img_admin_settings() {
  $form = array();
  $config = resp_img_settings();

  $suffixes = resp_img_suffix_load_all();
  $options = array();
  foreach ($suffixes as $suffix) {
    $options[$suffix->suffix] = $suffix->name . ' (' . $suffix->suffix . ')';
  }

  $form['resp_img_default_suffix'] = array(
    '#type' => 'select',
    '#title' => t('Default suffix'),
    '#description' => t('Specify the default suffix to use, ex. "_mobile". Make sure you also create the corresponding imagestyles.'),
    '#options' => $options,
    '#required' => TRUE,
    '#default_value' => $config->default_suffix,
  );

  $form['resp_img_block_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable responsive images for blocks.'),
    '#description' => t('If checked, the images within blocks will be responsive as well.'),
    '#default_value' => $config->block_enabled,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

function resp_img_admin_settings_submit($form, &$form_state) {
  resp_img_settings_save(
    check_plain($form_state['values']['resp_img_default_suffix']),
    $form_state['values']['resp_img_block_enabled']
  );
}

function resp_img_admin_loadfromtheme($form, &$form_state) {
  $enabled_themes = array();
  foreach(list_themes() as $key => $theme) {
    if ($theme->status) {
      $enabled_themes[$key] = $theme->info['name'];
    }
  }
  $form['theme'] = array(
    '#title' => t('Select theme'),
    '#type' => 'select',
    '#options' => $enabled_themes,
    '#required' => TRUE,
    '#description' => t('Read the breakpoints from this theme and create the suffixes'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create'),
  );
  return $form;

}

function resp_img_admin_loadfromtheme_validate($form, &$form_state) {
  if (empty($form_state['values']['theme'])) {
    form_set_error('theme', 'Please select a theme');
  }
}

function resp_img_admin_loadfromtheme_submit($form, &$form_state) {
  $themes = list_themes();
  $theme = $form_state['values']['theme'];
  if (isset($themes[$theme]->info['resp_img'])) {
    $breakpoints = $themes[$theme]->info['resp_img'];
    foreach ($breakpoints as $name => $breakpoint) {
      if (!resp_img_suffix_load($name)) {
        $suffix = new StdClass();
        $suffix->name = $name;
        $suffix->suffix = RESP_IMG_SEPARATOR . $name;
        $suffix->breakpoint = $breakpoint;
        $suffix->source = 'theme';
        resp_img_suffix_save($suffix);
      }
    }
  }
  system_theme_settings();
}

function resp_img_add_style_form($form, &$form_state) {
  module_load_include('inc', 'image', 'image.admin');
  $form['style'] = array(
    '#title' => t('Image style'),
    '#type' => 'select',
    '#options' => image_style_options(FALSE),
    '#required' => TRUE,
    '#description' => t('This image style will be cloned to create the responsive style'),
  );

  $form['base_name'] = array(
    '#type' => 'textfield',
    '#size' => '64',
    '#title' => t('Image style base name'),
    '#description' => t('The name is used in URLs for generated images. Use only lowercase alphanumeric characters, underscores (_), and hyphens (-).'),
    '#element_validate' => array('image_style_name_validate'),
    '#required' => TRUE,
  );
  $suffixes = resp_img_suffix_load_all();
  $options = array();
  foreach ($suffixes as $suffix) {
    $options[$suffix->suffix] = $suffix->name . ' (' . $suffix->suffix . ')';
  }
  $form['suffixes'] = array(
    '#title' => t('Suffixes'),
    '#type' => 'checkboxes',
    '#options' => $options,
    '#default_value' => drupal_map_assoc(array_keys($options)),
    '#description' => t('Select the suffixes to create an image style for'),
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create'),
  );
  return $form;
}

function resp_img_add_style_form_validate($form, &$form_state) {
  foreach (array_filter($form_state['values']['suffixes']) as $suffix) {
    if (image_style_load($form_state['values']['base_name'] . $suffix)) {
      form_set_error('suffixes', t('An image style with the name !name already exists', array('!name' => $form_state['values']['base_name'] . $suffix)));
    }
  }
}

function resp_img_add_style_form_submit($form, &$form_state) {
  $base = image_style_load($form_state['values']['style']);
  if (!isset($base['effects'])) {
    $base['effects'] = array();
  }
  foreach (array_filter($form_state['values']['suffixes']) as $suffix) {
    $new_style = array(
      'name' => $form_state['values']['base_name'] . $suffix,
      'effects' => $base['effects'],
    );
    image_style_save($new_style);
  }
  $form_state['redirect'] = 'admin/config/media/image-styles';
  drupal_set_message(t('The new styles have been created'));
}
