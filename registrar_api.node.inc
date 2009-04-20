<?php
// $Id$

/**
 * @file
 * Hooks to implement the domain custom content type
 */

/**
 * Implementation of hook_node_info().
 */
function registrar_api_node_info() {
  return array(
    'registrar_domain' => array(
      'name' => t('Domain'),
      'module' => 'registrar_api',
      'description' => "Domain Name",
    )
  );
}

/**
 * Implementation of hook_form().
 */
function registrar_api_form(&$node, $form_state) {
  $type = node_get_types('type', $node);

  // Define the form elements for the node's title and body.
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Domain Name'),
    '#required' => TRUE,
    '#default_value' => $node->title,
    '#weight' => -5
  );

  $form['years'] = array(
    '#type' => 'select',
    '#title' => t('Years'),
    '#default_value' => 1,
    '#options' => array(1 => 1, 2 => 2, 3 => 3, 4 => 4, 5 => 5, 6 => 6,
                        7 => 7, 8 => 8, 9 => 9, 10 => 10),
    '#required' => TRUE,
  );

  if ($node->nid) {
    $form += _registrar_api_get_form("node", $node->nid);
  }
  else {
    // this is a fresh node, so use the user profile settings
    global $user;
    $form += _registrar_api_get_form("user", $user->uid);
  }

  return $form;
}

/**
 * Implementation of hook_insert().
 */
function registrar_api_insert($node) {
  $status = registrar_api_create_domain($node->title, $node->years, $node->registrar_api_registrant, $node->registrar_api_admin, $node->registrar_api_tech, $node->registrar_api_aux_billing);

  $set_str = _registrar_api_get_set_str($node);
  db_query("INSERT INTO {registrar_api_node} SET $set_str, nid = %d",
    $node->nid);
}

/**
 * Implementation of hook_update().
 */
function registrar_api_update($node) {
  if ($node->op == "Sync") {
    // Get the contact info from the registrar
    $contacts = registrar_api_get_contacts($node->title);
    $set_str = _registrar_api_get_set_str(array(
      'registrar_api_registrant' => $contacts['registrant'],
      'registrar_api_tech' => $contacts['tech'],
      'registrar_api_admin' => $contacts['admin'],
      'registrar_api_aux_billing' => $contacts['aux_billing'],
    ));
  }
  else {
    if ($node->op == "Renew") {
      // Renew the domain
      registrar_api_renew_domain($node->title, $node->years);
    }

    // Update the contact information on the registrar
    registrar_api_set_contacts(
      $node->title, $node->years,
      $node->registrar_api_registrant,
      $node->registrar_api_admin,
      $node->registrar_api_tech,
      $node->registrar_api_aux_billing
    );

    $set_str = _registrar_api_get_set_str($node);
  }
  db_query("UPDATE {registrar_api_node} SET $set_str WHERE nid = %d",
    $node->nid);
}

/**
 * Implementation of hook_delete().
 */
function registrar_api_delete(&$node) {
  db_query("DELETE FROM {registrar_api_node} WHERE nid = %d", $node->nid);
}

/**
 * Implementation of hook_validate().
 */
function registrar_api_validate($node, &$form) {
  // TODO: Check the proper format on each field

  // Only validate on create
  if (strpos(request_uri(), '/node/add/') !== FALSE) {
    if (!registrar_api_check_domain($node->title)) {
      form_set_error('title', t('The specified domain name is not available.'));
    }
  }
}

/**
 * Implementation of hook_form_alter().
 */
function registrar_api_form_alter(&$form, $form_state, $form_id) {
  if ($form_id == 'registrar_domain_node_form') {

    // Move menu to the bottom
    $form['menu']['#weight'] = 5;
    // We need to do something hacky if CCK is being used.

    if (module_exists('content')) {
      $extra = variable_get('content_extra_weights_registrar_domain', array());
      $extra['menu'] = 5;
      variable_set('content_extra_weights_registrar_domain', $extra);
    }

    // Preview doesn't make sense in this context
    unset($form['buttons']['preview']);
    unset($form['buttons']['delete']);

    if (strpos(request_uri(), '/node/add/') === FALSE) {
      // We don't want the user to be able to edit the title of the domain
      $form['title']['#disabled'] = TRUE;
      $form['title']['#description'] = t('This field cannot be changed after the domain has been created.');
      $form['title']['#value'] = $form['title']['#default_value'];

      $form['years']['#description'] = t('Only used when renewing a domain');

      // Add renew and sync buttons
      $form['buttons']['Renew'] = array(
        '#type' => 'submit',
        '#value' => t('Renew'),
        '#submit' => array('node_form_submit'),
        '#weight' => 6,
      );
      $form['buttons']['Sync'] = array(
        '#type' => 'submit',
        '#value' => t('Sync'),
        '#submit' => array('node_form_submit'),
        '#weight' => 7,
      );
      $form['buttons']['sync_info'] = array(
        '#value' => '<div>' . t('The "sync" button will replace your contact information with that currently set by the registrar.') . '</div>',
        '#weight' => 10,
      );
    }
  }
}

/**
 * Implementation of hook_view().
 */
function registrar_api_view($node, $teaser = FALSE, $page = FALSE) {
  $contacts = registrar_api_get_node_contacts($node->nid);
  $content = theme('registrar_api_contacts', $contacts);
  $node->content['contacts'] = array('#value' => $content);
  return $node;
}

/**
 * Theme the domain contact info
 */
function theme_registrar_api_contacts($contacts) {
  $content = "";
  foreach ($contacts as $type => $values) {
    if (in_array($type, array('registrant', 'admin', 'tech', 'aux_billing'))) {
      $content .= "<h2>$type</h2>";
      foreach ($values as $key => $value) {
        $content .=
          '<div class="field-item">' .
          '  <div class="field-label-inline">' . $key . '</div>' .
          $value .
          '</div>';
      }
    }
  }

  return $content;
}
