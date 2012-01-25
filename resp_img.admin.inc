<?php

/**
 * @file
 * Admin settings
 */

function resp_img_admin_settings() {
  $form = array();

  $form['resp_img_default_suffix'] = array(
    '#type' => 'textfield',
    '#title' => t('Default suffix'),
    '#description' => t('Specify the default suffix to use.'),
    '#required' => FALSE,
    '#default_value' => variable_get('resp_img_default_suffix', ''),
    '#size' => 20,
  );

  return system_settings_form($form);
}

