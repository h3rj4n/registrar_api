<?php

/**
 * Callback function for the registrar_api admin settings form.
 */
function registrar_api_settings_form($form, &$form_state) {
  
  // Select available TLDs
  $tlds = variable_get('registrar_api_iana_tlds', FALSE);
  $form['iana'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available TLDs'),
    '#collapsible' => TRUE,
    '#description' => t('Select the supported TLDs'),
  );
  if (!$tlds) {
    $form['iana']['msg'] = array(
      '#theme' => 'html_tag',
      '#tag' => 'p',
      '#value' => t('To start using registrar API it\'s needed to download the TLDs database from <a href="@url">@url</a>', array('@url' => REGISTRAR_API_IANA_TLDS_URL)),
    );
    $form['iana']['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Download IANA database now'
    );
    $form['#submit'] = array('registrar_api_settings_initialize_tlds');
    return $form;
  }

  $tlds_enabled = variable_get('registrar_api_iana_tlds_enabled', array());
  if (!empty($tlds_enabled)) {
    $form['iana']['#collapsed'] = TRUE;
  }
  $options = array_merge($tlds_enabled, $tlds);
  $form['iana']['tlds'] = array(
    '#type' => 'checkboxes',
    '#options' => $tlds,
    '#value' => $tlds_enabled,
  );

  $form['registrars'] = array(
    '#type' => 'fieldset',
    '#title' => t('Select registrars'),
    '#description' => t('Select the registrar to use by default and optionally a specific registrar per TLD.'),
    '#collapsible' => TRUE,
  );
  $registrars = registrar_api_get_registrars();
  $options = array();
  foreach ($registrars as $k => $v) {
    $options[$k] = $v['name'];
  }
  $default = variable_get('registrar_api_registrar_default', '');
  if ($default != '') {
    $form['registrars']['#collapsed'] = TRUE;
  }
  $form['registrars']['default_registrar'] = array(
    '#type' => 'select',
    '#title' => t('Default'),
    '#default_value' => $default,
    '#options' => $options,
  );
  
  $form['registrars']['tld_registrar'] = array(
    '#type' => 'item',
    '#tree' => TRUE,
  );
  $options = array_merge(array('' => t('Default')), $options);
  foreach ($tlds_enabled as $tld) {
    $items = array(
      'tld' => array('#value' => $tld),
      'registrar' => array(
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => variable_get('registrar_api_registrar_'.$tld, ''),
      ),
    );
    $form['registrars']['tld_registrar'][$tld] = $items;
  }

  $form['nameservers'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default nameservers'),
    '#description' => t('Select the nameservers to use by default.'),
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );
  $nameservers = variable_get('registrar_api_nameservers_default', array());
  if (!empty($nameservers)) {
    $form['nameservers']['#collapsed'] = TRUE;
  }
  for ($i=0;$i<5;$i++) {
    $form['nameservers'][$i] = array(
      '#type' => 'textfield',
      '#default_value' => isset($nameservers[$i])?$nameservers[$i]:'',
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration')
  );

  return $form;
}

/**
 * Validate callback.
 */
function registrar_api_settings_form_validate($form, &$form_state) {
  $values = &$form_state['values'];

  // Validate nameservers.
  foreach ($values['nameservers'] as $i => $value) {
    if ($value == '') {
      continue;
    }
    if (!_registrar_api_check_valid_nameserver($value)) {
      form_error($form['nameservers'][$i], t('Invalid nameserver.'));
    }
  }
}

/**
 * Submit callback for the settings form when the list of TLDs is not available.
 */
function registrar_api_settings_initialize_tlds($form, $form_state) {
  $result = drupal_http_request(REGISTRAR_API_IANA_TLDS_URL);
  if ($result->code != 200) {
    form_error($form['iana'], $result->status_message);
  }
  $tlds = explode("\n", trim($result->data));
  array_shift($tlds);
  $tlds = drupal_map_assoc($tlds);
  variable_set('registrar_api_iana_tlds', $tlds);
}

/**
 * Submit callback.
 */
function registrar_api_settings_form_submit($form, $form_state) {
  $values = &$form_state['values'];
  // Enabled IANA TLDs.
  $tlds = variable_get('registrar_api_iana_tlds', array());
  $enabled = array_intersect_assoc($tlds, $values['tlds']);
  variable_set('registrar_api_iana_tlds_enabled', $enabled);

  // Default and per tld registrars.
  variable_set('registrar_api_registrar_default', $values['default_registrar']);
  foreach ($values['tld_registrar'] as $tld => $value) {
    variable_set('registrar_api_registrar_'.$tld, $value['registrar']);
  }

  // Default nameservers.
  variable_set('registrar_api_nameservers_default', $values['nameservers']);
}