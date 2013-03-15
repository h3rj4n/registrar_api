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
 *   - actions, an array of the actions the new provider supports. If key
 *     value array, the key of the array is the action, the value the
 *     function that should be called.
 *     At time of release the registrar_api supports the following:
 *       - domain_check
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
      'actions' => array(
        'domain_check' => 'registrar_openprovider_domain_check',
        'domain_read' => 'registrar_openprovider_domain_read',
        'domain_create' => 'registrar_openprovider_domain_create',
        'domain_update' => 'registrar_openprovider_domain_update',
        'domain_delete' => 'registrar_openprovider_domain_delete',
      ),
    )
  );
}

function hook_registrar