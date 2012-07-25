<?php

/**
 * @file
 * Admin settings
 */

/**
 * Admin form
 */
function resp_img_admin_suffixes($form, &$form_state, $theme_key = '') {
  if ($theme_key == '') {
    $theme_key = variable_get('theme_default', 'stark');
  }
  $form = array();

  $settings = resp_img_settings();
  $multipliers = array();
  if (isset($settings->multipliers) && !empty($settings->multipliers)) {
    $multipliers = drupal_map_assoc(array_values($settings->multipliers));
  }

  $form['suffixes'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#theme_key' => $theme_key,
    '#theme' => 'resp_img_admin_suffixes_table',
    '#multipliers' => $multipliers,
  );

  $suffixes = resp_img_suffix_load_all($theme_key);
  foreach ($suffixes as $key => $suffix) {
    $form['suffixes'][$key] = array(
      '#suffix_data' => $suffix,
      'name' => array(
        '#type' => 'textfield',
        '#default_value' => $suffix->name,
        '#disabled' => TRUE,
        '#size' => 20,
      ),
      'breakpoint' => array(
        '#type' => 'textfield',
        '#default_value' => $suffix->breakpoint,
        '#disabled' => $suffix->source === 'theme',
        '#size' => empty($multipliers) ? 60 : 30,
      ),
      'weight' => array(
        '#type' => 'textfield',
        '#size' => 4,
        '#default_value' => isset($suffix->weight) ? $suffix->weight : 0,
        '#attributes' => array('class' => array('suffixes-weight')),
      ),
    );
    // Add multipliers checkboxes if needed.
    if (!empty($multipliers)) {
      $form['suffixes'][$key]['multipliers'] = array(
        '#type' => 'checkboxes',
        '#default_value' => isset($suffix->multipliers) ? $suffix->multipliers : array(),
        '#options' => $multipliers,
      );
    }
  }

  // Add empty row
  $form['suffixes']['new'] = array(
    'name' => array(
      '#type' => 'textfield',
      '#default_value' => '',
      '#size' => 20,
    ),
    'breakpoint' => array(
      '#type' => 'textfield',
      '#default_value' => '',
      '#size' => empty($multipliers) ? 60 : 30,
    ),
    'weight' => array(
      '#type' => 'textfield',
      '#size' => 4,
      '#default_value' => 0,
      '#attributes' => array('class' => array('suffixes-weight')),
    ),
  );
  // Add multipliers checkboxes if needed.
  if (!empty($multipliers)) {
    $form['suffixes']['new']['multipliers'] = array(
      '#type' => 'checkboxes',
      '#default_value' => array(),
      '#options' => $multipliers,
    );
  }

  // Buttons
  $form['buttons'] = array(
    '#type' => 'container',
  );

  // Submit button
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Theme form as table.
 */
function theme_resp_img_admin_suffixes_table($variables) {
  drupal_add_css(drupal_get_path('module', 'resp_img') . '/css/resp_img.admin.css');
  $form = $variables['form'];
  $rows = array();
  $link_attributes = array(
    'attributes' => array(
      'class' => array('image-style-link'),
    ),
  );
  foreach (element_children($form) as $key) {
    $form[$key]['weight']['#attributes']['class'] = array('suffixes-weight');
    $row = array();
    $row[] = drupal_render($form[$key]['name']);
    $row[] = drupal_render($form[$key]['breakpoint']);
    $row[] = drupal_render($form[$key]['multipliers']);
    if ($key != 'new') {
      $suffix = $form[$key]['#suffix_data'];
      $row[] = $suffix->source . ' (' . $suffix->source_type . ')';
      $row[] = $suffix->status ? t('Enabled') : t('Disabled');
      $row[] = l($suffix->status ? t('Disable') : t('Enable'), 'admin/config/media/resp_img/' . $form['#theme_key'] . '/' . ($suffix->status ? 'disable' : 'enable'). '/' . $key, $link_attributes);
      if ($suffix->source == 'user') {
        $row[] = l(t('Delete'), 'admin/config/media/resp_img/' . $form['#theme_key'] . '/delete/' . $key, $link_attributes);
      }
      else {
        $row[] = '';
      }
      $class= 'resp-img-status-' . ($suffix->status ? 'enabled' : 'disabled');
    }
    else {
      $row[] = '';
      $row[] = '';
      $row[] = '';
      $row[] = '';
      $class = 'resp-img-status-new';
    }
    $row[] = drupal_render($form[$key]['weight']);
    $rows[] = array(
      'data' => $row,
      'class' => array('draggable', $class),
    );
   }

  $header =  array(
    t('Name'),
    t('Breakpoint, @media ...'),
    t('Multipliers'),
    t('Source'),
    t('Status'),
    array('data' => t('Operations'), 'colspan' => 2),
    t('Weight'),
  );

  $output = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'resp-img-suffixes')));
  drupal_add_tabledrag('resp-img-suffixes', 'order', 'sibling', 'suffixes-weight');

  return $output;
}

/**
 * Admin form validation.
 */
function resp_img_admin_suffixes_validate($form, &$form_state) {
  if (strpos($form_state['triggering_element']['#id'], 'remove') === FALSE) {
    $suffixes = $form_state['values']['suffixes'];
    if (!empty($suffixes)) {
      $unique = array();
      foreach ($suffixes as $key => $suffixdata) {
        if (!empty($suffixdata['name'])) {
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

/**
 * Admin form submit.
 */
function resp_img_admin_suffixes_submit($form, &$form_state) {
  $suffixes = $form_state['values']['suffixes'];
  if (!empty($suffixes)) {
    foreach ($suffixes as $suffixname => $suffixdata) {
      if (!empty($suffixdata['name'])) {
        $suffix = resp_img_suffix_load_by_fullkey($suffixname);
        if ($suffix) {
          $suffix->breakpoint = $suffixdata['breakpoint'];
          $suffix->weight = $suffixdata['weight'];
          $suffix->multipliers = $suffixdata['multipliers'];
          resp_img_suffix_save($suffix);
        }
        else {
          $suffix = new StdClass();
          $suffix->name = $suffixdata['name'];
          $suffix->suffix = RESP_IMG_SEPARATOR . $suffix->name;
          $suffix->breakpoint = $suffixdata['breakpoint'];
          $suffix->source = 'user';
          $suffix->source_type = 'custom';
          $suffix->theme = $form_state['complete_form']['suffixes']['#theme_key'];
          $suffix->weight = $suffixdata['weight'];
          $suffix->status = TRUE;
          $suffix->multipliers = $suffixdata['multipliers'];
          resp_img_suffix_save($suffix);
        }
      }
    }
  }
}

/**
 * Admin action form: enable, disable, delete
 */
function resp_img_admin_suffix_actions_form($form, &$form_state, $theme_key, $action, $breakpoint) {
  $form_state['theme_key'] = $theme_key;
  $form_state['action'] = $action;
  $form_state['breakpoint'] = $breakpoint;
  $question = t('Are you sure you want to %action %breakpoint', array(
    '%action' => $action,
    '%breakpoint' => $breakpoint,
  ));
  $path = 'admin/config/media/resp_img/' . $theme_key;
  return confirm_form($form, $question, $path, '');
}

/**
 * Admin action form submit
 */
function resp_img_admin_suffix_actions_form_submit($form, &$form_state) {
  $theme_key = $form_state['theme_key'];
  $action = $form_state['action'];
  $breakpoint = $form_state['breakpoint'];
  switch ($action) {
    case 'delete':
      resp_img_suffix_delete($breakpoint);
      break;
    case 'enable':
    case 'disable':
      resp_img_suffix_toggle_status($breakpoint);
      break;
  }
  $form_state['redirect'] = 'admin/config/media/resp_img/' . $theme_key;
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
  $suffixes = resp_img_suffix_load_all_active();
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
