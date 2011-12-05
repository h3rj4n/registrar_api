<?php

/**
 * @file
 * Hooks to implement the custom content types
 */

/**
 * Implementation of hook_node_info().
 *
 * Note 'module' key isn't actually a real module name but the prefix we want
 * for the hooks. It has been renamed to 'base' in Drupal 7.
 */
function registrar_api_node_info() {
  return array(
    'registrar_api_contact' => array(
      'name' => t('Contact Handle'),
      'module' => 'registrar_api_contact',
      'description' => 'A contact handle representing a contact',
      'has_body' => FALSE,
    ),
    'registrar_api_domain' => array(
      'name' => t('Domain'),
      'module' => 'registrar_api_domain',
      'description' => 'A domain name',
      'title_label' => t('Domain'),
      'has_body' => FALSE,
    ),
  );
}

/**
 * Implementation of hook_form() for contact content type.
 */
function registrar_api_contact_form(&$node, $form_state) {
  $type = node_get_types('type', $node);

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => check_plain($type->title_label),
    '#required' => TRUE,
    '#default_value' => $node->title,
    '#size' => 20,
  );

  /* DB schema fields */
  $form['uid'] = array(
    '#type' => 'textfield',
    '#title' => t('Users table uid'),
    '#required' => TRUE,
    '#default_value' => isset($node->uid) ? $node->uid : NULL,
  );
  $form['company_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Company Name'),
    '#required' => TRUE,
    '#size' => 60,
    '#maxlength' => 60,
    '#autocomplete_path' => 'registrar/company/autocomplete',
    '#default_value' => isset($node->company_name) ? $node->company_name : ''
  );
  $form['firstname'] = array(
    '#type' => 'textfield',
    '#title' => t('Firstname'),
    '#required' => TRUE,
    '#size' => 60,
    '#maxlength' => 60,
    '#default_value' => isset($node->firstname) ? $node->firstname : ''
  );
  $form['lastname'] = array(
    '#type' => 'textfield',
    '#title' => t('Lastname'),
    '#required' => TRUE,
    '#size' => 60,
    '#maxlength' => 60,
    '#default_value' => isset($node->lastname) ? $node->lastname : ''
  );
  $form['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email address'),
    '#required' => TRUE,
    '#size' => 60,
    '#maxlength' => 255,
    '#default_value' => isset($node->email) ? $node->email : ''
  );

  $settings = _registrar_api_location_settings();
  $location = isset($node->location) ? $node->location : array();
  $form['location'] = location_form($settings, $location);

  return $form;
}

/**
 * Implementation of hook_form() for the domain content type.
 */
function registrar_api_domain_form(&$node, $form_state) {
  $type = node_get_types('type', $node);

  /* DB schema fields */
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => check_plain($type->title_label),
    '#required' => TRUE,
    '#default_value' => $node->title,
    '#size' => 20,
  );
  $form['tld'] = array(
    '#type' => 'textfield',
    '#title' => t('Top level domain'),
    '#required' => TRUE,
    '#default_value' => isset($node->tld) ? $node->tld : ''
  );

  // Contact handlers.
  $form['handles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Contacts'),
    '#collapsible' => TRUE,
    '#weight' => -10,
  );
  // TODO: Default contacts are known once the tld is set.
  $form['handles'] += _registrar_api_domain_handles_form($node);

  return $form;
}

/**
 * Implementation of hook_validate().
 */
function registrar_api_contact_validate($node, &$form) {
  // TODO: Validate uid

  // TODO: Validate contacts.
  if (!user_access('override default contacts')) {
     
  }
}

/**
 * Implementation of hook_validate().
 *
 * If a handle doesn't exist, show an error.
 * A domain needs those three handles, otherwise we can't register them
 */
function registrar_api_domain_validate($node, &$form) {
  $ctypes = array(
    'Registrar'      => 'registrar_handle',
    'Administrative' => 'adm_handle',
    'Technical'      => 'tech_handle'
  );
  foreach ($ctypes as $name => $ctype) {
    // Check if the registrar handle exists
    $result = db_query("SELECT COUNT(contact.nid) FROM {registrar_api_contact} contact LEFT JOIN {node} node on ( node.nid = contact.nid ) WHERE node.title='%s'", $form[$ctype]['#value']);
    if (db_result($result) != 1) {
      form_set_error($ctype, t('!name handle does not exist.', array('!name' => $name)));
    }
  }
}

/**
 * Implementation of hook_insert().
 */
function registrar_api_contact_insert($node) {
  drupal_write_record('registrar_api_contact', $node);
  drupal_write_record('registrar_api_user_handles', $node);
  location_save_locations($node->location, array('nid' => $node->nid, 'vid' => $node->vid));
}

/**
 * Implementation of hook_insert().
 *
 * This function is also called by registrar_api_domain_update().
 */
function registrar_api_domain_insert($node, $update = array()) {
  $record = array(
    'nid' => $node->nid,
    'tld' => $node->tld
  );
  foreach (array('registrar_handle', 'adm_handle', 'tech_handle') as $ctype) {
    $contact = db_result(db_query("SELECT contact.nid FROM {registrar_api_contact} contact LEFT JOIN {node} node on ( node.nid = contact.nid ) WHERE node.title='%s'", $node->$ctype));
    $record[$ctype] = $contact;
  }
  drupal_write_record('registrar_api_domain', $record, $update);
}

/**
 * Implementation of hook_update().
 */
function registrar_api_contact_update($node) {
  drupal_write_record('registrar_api_contact', $node, 'nid');
  drupal_write_record('registrar_api_user_handles', $node, 'nid');
  location_save_locations($node->location, array('nid' => $node->nid, 'vid' => $node->vid));
}

/**
 * Implementation of hook_update().
 */
function registrar_api_domain_update($node) {
  // TODO: module_invoke_all('update_contact', $node);
  registrar_api_domain_insert($node, 'nid');
}

/**
 * Implementation of hook_view().
 */
function registrar_api_contact_view($node, $teaser = FALSE, $page = FALSE) {
  $node->content['Contact'] = array(
    '#value' => _registrar_api_format_contact_info($node, TRUE),
    '#weight' => 1,
  );
  $settings = _registrar_api_location_settings();
  if (isset($settings['display']['teaser']) && isset($settings['display']['full'])) {
    if (($teaser && $settings['display']['teaser']) || (!$teaser && $settings['display']['full'])) {
      $node->content['location'] = location_display($settings, $node->location);
    }
  }
  return $node;
}

function registrar_api_domain_view($node, $teaser = FALSE, $page = FALSE) {
  $node->content['Domain'] = array(
    '#value' => $node->title,
    '#weight' => 1,
  );
  return $node;
}

/**
 * Implementation of hook_load().
 */
function registrar_api_contact_load($node) {
  $contact = db_fetch_object(db_query("SELECT * FROM {registrar_api_contact} WHERE nid = %d", $node->nid));
  $contact->location = location_load_locations($node->vid);
  return $contact;
}

/**
 * Implementation of hook_load().
 */
function registrar_api_domain_load($node) {
  // Fetch the names iso the id's for the handles
  // Now we can autocomplete the names iso the id's
  // @see registrar_api_domain_update
  // @see registrar_api_domain_insert
  $query = "
  SELECT domain.nid, domain.tld, tech_handle.title AS tech_handle,
    adm_handle.title AS adm_handle,registrar_handle.title as registrar_handle
  FROM {registrar_api_domain} AS domain
  LEFT JOIN ({registrar_api_contact} AS domain_contact, {node} AS tech_handle)
    ON (domain.tech_handle = domain_contact.nid AND domain_contact.nid = tech_handle.nid)
  LEFT JOIN ( {registrar_api_contact} AS adm_contact, {node} AS adm_handle)
    ON (domain.adm_handle = adm_contact.nid AND adm_contact.nid = adm_handle.nid)
  LEFT JOIN ( {registrar_api_contact} AS registrar_contact, {node} AS registrar_handle)
    ON (domain.registrar_handle = registrar_contact.nid AND registrar_contact.nid = registrar_handle.nid)
  WHERE domain.nid=%d
  ";
  $domain = db_fetch_object(db_query($query, $node->nid));
  return $domain;
}

/**
 * There is no implementation of hook_delete()
 * Handles may not be deleted, because the can be used by domains
 * If there are domains active, and the handle is deleted, we may have a problem with renewing the domain
 */

/**
 * Implementation of hook_form_alter().
 */
function registrar_api_form_alter(&$form, $form_state, $form_id) {
  if ($form_id == 'registrar_api_contact_node_form' || $form_id == 'registrar_api_domain_node_form') {
    // Move menu to the bottom
    $form['menu']['#weight'] = 5;

    // We need to do something hacky if CCK is being used.
    if (module_exists('content')) {
      $extra = variable_get('content_extra_weights_registrar_contact', array());
      $extra['menu'] = 5;
      variable_set('content_extra_weights_registrar_contact', $extra);
    }

    // Preview doesn't make sense in this context
    unset($form['buttons']['preview']);
    unset($form['buttons']['delete']);
  }
}

/**
 * Helper to obtain default settings for location form.
 *
 * @see hook_registrar_api_location_settings_alter().
 */
function _registrar_api_location_settings() {
  $settings = array(
    'multiple' => array(
      'min' => 1,
      'max' => 1,
      'add' => 1
    ),
    'form' => array(
      'weight' => 1,
      'collapsible' => 0,
      'collapsed' => 0,
      'fields' => array(
        'name' => array(
          'collect' => 0,
          'default' => NULL,
          'weight' => 2
        ),
        'country' => array(
          'collect' => 1,
          'default' => '',
          'weight' => 4
        ),
        'province' => array(
          'collect' => 2,
          'default' => NULL,
          'weight' => 6
        ),
        'city' => array(
          'collect' => 2,
          'default' => NULL,
          'weight' => 8
        ),
        'street' => array(
          'collect' => 2,
          'default' => NULL,
          'weight' => 10
        ),
        'additional' => array(
          'collect' => 1,
          'default' => NULL,
          'weight' => 12
        ),
        'postal_code' => array(
          'collect' => 2,
          'default' => NULL,
          'weight' => 14
        ),
        'locpick' => array(
          'collect' => 0,
          'weight' => 20,
        ),
        'phone' => array(
          'collect' => 2,
          'default' => NULL,
          'weight' => 25
        ),
        'fax' => array(
          'collect' => 1,
          'default' => NULL,
          'weight' => 30
        )
      )
    ),
    'display' => array(
      'weight' => 1,
      'hide' => array(
        'country' => 0,
        'country_name' => 0,
        'province' => 0,
        'province_name' => 0,
        'city' => 0,
        'name' => 0,
        'street' => 0,
        'additional' => 0,
        'postal_code' => 0,
        'locpick' => 0,
        'fax' => 0,
        'phone' => 0,
        'map_link' => 0,
        'coords' => 0,
      ),
      'teaser' => 1,
      'full' => 1,
    ),
    'rss' => array(
      'mode' => FALSE
    )
  );
  drupal_alter('registrar_api_location_settings', $settings);

  return $settings;
}