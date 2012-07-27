<?php

/**
 * @file
 * Admin settings
 */

/**
 * Admin form
 */
function resp_img_admin_suffixes($form, &$form_state, $breakpoint_group_name = '') {

  // Global is the same as no group name
  if ($breakpoint_group_name == 'global') {
    $breakpoint_group_name = '';
  }
  $form_state['group_name'] = $breakpoint_group_name;

  $form = array();

  $settings = resp_img_settings();
  $multipliers = array();
  if (isset($settings->multipliers) && !empty($settings->multipliers)) {
    $multipliers = drupal_map_assoc(array_values($settings->multipliers));
    if (array_key_exists('1x', $multipliers)) {
      unset($multipliers['1x']);
    }
  }

  $form['suffixes'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#theme' => 'resp_img_admin_suffixes_table',
    '#multipliers' => $multipliers,
    '#group_name' => $breakpoint_group_name,
  );

  $suffixes = array();
  $breakpoint_group = resp_img_breakpoint_group_load($breakpoint_group_name);
  if ($breakpoint_group_name == '') {
    $suffixes = resp_img_suffix_load_all();
  }
  else {
    foreach ($breakpoint_group->breakpoints as $suffix_name) {
      $suffixes[$suffix_name] = resp_img_suffix_load_by_fullkey($suffix_name);
    }
  }

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
        '#disabled' => $suffix->source_type == 'theme',
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
        '#default_value' => (isset($suffix->multipliers) && is_array($suffix->multipliers)) ? $suffix->multipliers : array(),
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
  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  if ($breakpoint_group_name != '') {
    $form['buttons']['editlink'] = array(
      '#type' => 'markup',
      '#markup' => l('Edit group breakpoints', 'admin/config/media/resp_img/groups/' . $breakpoint_group_name . '/edit', array(
        'query' => drupal_get_destination(),
      )),
    );
    if ($breakpoint_group->type == RESP_IMG_SOURCE_TYPE_CUSTOM) {
      $form['buttons']['deletelink'] = array(
        '#type' => 'markup',
        '#markup' => l('Delete this group', 'admin/config/media/resp_img/groups/' . $breakpoint_group_name . '/delete', array(
          'query' => drupal_get_destination(),
        )),
      );
    }
  }

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
      $row[] = l($suffix->status ? t('Disable') : t('Enable'), 'admin/config/media/resp_img/' . $form['#group_name'] . '/' . ($suffix->status ? 'disable' : 'enable'). '/' . $key, $link_attributes);
      if ($suffix->source == 'user') {
        $row[] = l(t('Delete'), 'admin/config/media/resp_img/' . $form['#group_name'] . '/delete/' . $key, $link_attributes);
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
  $saved_suffixes = array();
  if (!empty($suffixes)) {
    foreach ($suffixes as $suffixname => $suffixdata) {
      if (!empty($suffixdata['name'])) {
        $suffix = resp_img_suffix_load_by_fullkey($suffixname);
        if ($suffix) {
          $suffix->breakpoint = $suffixdata['breakpoint'];
          $suffix->weight = $suffixdata['weight'];
          $suffix->multipliers = $suffixdata['multipliers'];
          resp_img_suffix_save($suffix);
          $saved_suffixes[] = $suffixname;
        }
        else {
          $suffix = new StdClass();
          $suffix->name = $suffixdata['name'];
          $suffix->suffix = RESP_IMG_SEPARATOR . $suffix->name;
          $suffix->breakpoint = $suffixdata['breakpoint'];
          $suffix->source = 'user';
          $suffix->source_type = 'custom';
          $suffix->weight = $suffixdata['weight'];
          $suffix->status = TRUE;
          $suffix->multipliers = $suffixdata['multipliers'];
          $suffix = resp_img_suffix_save($suffix);
          $saved_suffixes[] = resp_img_suffix_config_name($suffix);
        }
      }
    }
    $group_name = $form_state['group_name'];
    if ($group_name != '') {
      $group = resp_img_breakpoint_group_load($group_name);
      if ($group) {
        $group->breakpoints = $saved_suffixes;
        resp_img_breakpoint_group_save($group);
        dpm($group);
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

function resp_img_admin_breakpoint_group_edit_form($form, &$form_state, $machine_name = '') {
  $form = array();
  $group = resp_img_breakpoint_group_load($machine_name);

  $form_state['#breakpoint_group'] = $group;
  $is_new = $machine_name == '';
  $form_state['#is_new'] = $is_new;

  $form['name'] = array(
    '#type' => 'textfield',
    '#size' => '64',
    '#title' => t('group name'),
    '#required' => TRUE,
    '#default_value' => isset($group->name) ? $group->name : '',
    '#disabled' => !$is_new,
  );

  $form['machine_name'] = array(
    '#type' => 'machine_name',
    '#size' => '64',
    '#title' => t('Machine name'),
    '#required' => TRUE,
    '#default_value' => isset($group->machine_name) ? $group->machine_name : '',
    '#disabled' => !$is_new,
    '#machine_name' => array(
      'exists' => 'resp_img_breakpoint_group_name_exists',
    ),
  );

  $suffixes = resp_img_suffix_load_all();
  foreach ($suffixes as $suffix_name => $suffix) {
    $options[$suffix_name] = $suffix->name . ' [' . $suffix->breakpoint . ']';
  }

  $form['breakpoints'] = array(
    '#title' => 'Select the breakpoints you want to use in this group',
    '#type' => 'checkboxes',
    '#options' => $options,
    '#default_value' => isset($group->breakpoints) ? drupal_map_assoc($group->breakpoints) : array(),
    '#required' => TRUE,
  );

  // Buttons
  $form['buttons'] = array(
    '#type' => 'container',
  );

  // Submit button
  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  if (!$is_new && $group->type == RESP_IMG_SOURCE_TYPE_CUSTOM) {
    $form['buttons']['deletelink'] = array(
      '#type' => 'markup',
      '#markup' => l('Delete this group', 'admin/config/media/resp_img/groups/' . $group->machine_name . '/delete', array(
        'query' => drupal_get_destination(),
      )),
    );
  }

  return $form;
}

function resp_img_admin_breakpoint_group_edit_form_validate($form, &$form_state) {
  $name = $form_state['values']['machine_name'];
  $label = $form_state['values']['name'];
}

function resp_img_admin_breakpoint_group_edit_form_submit($form, &$form_state) {
  $machine_name = $form_state['values']['machine_name'];
  $name = $form_state['values']['name'];
  $breakpoints = array();
  foreach ($form_state['values']['breakpoints'] as $breakpoint => $status) {
    if ($status) {
      $breakpoints[] = $breakpoint;
    }
  }
  $is_new = $form_state['#is_new'];

  if ($is_new) {
    $new_group = resp_img_breakpoint_group_empty_object();
    $new_group->machine_name = $machine_name;
    $new_group->name = $name;
    $new_group->type = RESP_IMG_SOURCE_TYPE_CUSTOM;
    $new_group->breakpoints = $breakpoints;
    resp_img_breakpoint_group_save($new_group);
    menu_cache_clear_all();
    $form_state['redirect'] = 'admin/config/media/resp_img/groups/' . $machine_name;
    drupal_set_message(t('The new group have been created'));
  }
  else {
    $existing_group = resp_img_breakpoint_group_load($name);
    $existing_group->breakpoints = $breakpoints;
    resp_img_breakpoint_group_save($existing_group);
  }
}

/**
 * Delete a group.
 */
function resp_img_admin_breakpoint_group_delete_form($form, &$form_state, $machine_name) {
  $group = resp_img_breakpoint_group_load($machine_name);
  $form_state['machine_name'] = $machine_name;
  $question = t('Are you sure you want to delete %group', array(
    '%group' => $group->name,
  ));
  $path = 'admin/config/media/resp_img/groups/' . $machine_name;
  return confirm_form($form, $question, $path, '');
}

/**
 * Delete a group.
 */
function resp_img_admin_breakpoint_group_delete_form_submit($form, &$form_state) {
  $machine_name = $form_state['machine_name'];
  resp_img_breakpoint_group_delete_by_name($machine_name);
  menu_cache_clear_all();
  $form_state['redirect'] = 'admin/config/media/resp_img/groups';
}
