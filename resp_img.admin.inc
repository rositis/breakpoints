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
    '#description' => t('Specify the default suffix to use, ex. "_mobile". Make sure you also create the corresponding imagestyles.'),
    '#required' => TRUE,
    '#default_value' => variable_get('resp_img_default_suffix', ''),
    '#size' => 20,
  );

  $form['resp_img_forceredirect'] = array(
    '#type' => 'select',
    '#title' => t('Force redirect'),
    '#description' => t('Force redirect if cookie is not set.'),
    '#required' => TRUE,
    '#default_value' => variable_get('resp_img_forceredirect', FALSE),
    '#options' => array(0 => t('No'), 1 => ('Yes'),),
  );

  $form['resp_img_forceresize'] = array(
    '#type' => 'select',
    '#title' => t('Force resize'),
    '#description' => t('Resize images when browser windows resizes.'),
    '#required' => TRUE,
    '#default_value' => variable_get('resp_img_forceresize', FALSE),
    '#options' => array(0 => t('No'), 1 => ('Yes'),),
  );

  $form['resp_img_reloadonresize'] = array(
    '#type' => 'select',
    '#title' => t('Reload on resize'),
    '#description' => t('Reload the page when browser windows resizes, use wisely!'),
    '#required' => TRUE,
    '#default_value' => variable_get('resp_img_reloadonresize', FALSE),
    '#options' => array(0 => t('No'), 1 => ('Yes'),),
  );

  $form['resp_img_purgeexpire'] = array(
    '#type' => 'select',
    '#title' => t('Advanced support for purge/expire'),
    '#description' => t('Clear all variants using purge/expire modules.'),
    '#required' => TRUE,
    '#default_value' => variable_get('resp_img_purgeexpire', FALSE),
    '#options' => array(0 => t('No'), 1 => ('Yes'),),
  );

  return system_settings_form($form);
}

