<?php

/**
 * Register a new API / domain provider. This hook lets registrar_api
 * know what functions the new provider supports.
 *
 * @return array
 *   An array keyed by the new provider name. See the function below
 *   as example.
 *   The array should contain the following:
 *   - label (required)
 *   - description (optional)
 *   - module, the module name wich this hook implements.
 *     Default is set to registrar_<key name of array>
 *   - actions, an array of the actions the new provider supports. To
 *     support these actions you'll have to implement the right hooks.
 *     At time of release the registrar_api supports the following:
 *       - domain_check
 *       - domain_check_multiple
 *       - domain_read
 *       - domain_create
 *       - domain_update
 *       - domain_delete
 */
function hook_registrar_api_info() {
  return array(
    'openprovider' => array(
      'label' => 'OpenProvider',
      'description' => 'OpenProvider support',
      'module' => 'registrar_openprovider',
      'callbacks' => array(
        'domain_check',
        'domain_check_multiple',
        'domain_read',
        'domain_create',
        'domain_update',
        'domain_delete',
      ),
    )
  );
}

function hook_registrar_api_domain_check() {
  
}

function hook_registrar_api_domain_check_multiple() {
  
}

function hook_registrar_api_domain_read() {
  
}

function hook_registrar_api_domain_create() {
  
}

function hook_registrar_api_domain_update() {
  
}

function hook_registrar_api_domain_delete() {
  
}