<?php

/**
 * Implements hook_entity_info().
 */
function registrar_api_entity_info() {
  return array(
    'registrar' => array(
      'label' => 'Registrar',
      'entity class' => 'RegistrarEntity',
      'entity controller' => 'RegistrarController',
      'base table' => 'registrar',
      'entity keys' => array(
        'id' => 'did',
      ),
      'module' => 'registrar',
      'fieldable' => TRUE,
    ),
  );
}

/**
 * Load a single domain object
 *
 * @param int $did
 *   The domain id wich should be loaded
 * @param bool $reset
 *   Wether the load should use cache or not
 */
function registrar_api_load($did, $reset = FALSE) {
  $domain = register_api_load_multiple(array($did), array(), $reset);
  return reset($domain);
}

/**
 * Load multiple domain objects
 *
 * @param array $dids
 *   Multiple domain id's wich should be loaded.
 * @param array $conditions
 * @param bool $reset
 *   Wether the load should not use (entity) cache.
 */
function registrar_api_load_multiple($dids = array(), $conditions = array(), $reset = FALSE) {
  return entity_load('registrar', $dids, $conditions, $reset);
}

/**
 * Save a domain object
 */
function registrar_api_save($domain) {
  entity_save('registrar', $domain);
}

/**
 * Delete a domain object
 *
 * @param object $domain
 *   A domain object containing the domain id
 */
function registrar_api_delete($domain) {
  entity_delete('registrar', entity_id('registrar', $domain));
}

/**
 * Delete multiple domains
 *
 * @param array $dids
 *   An array containing all the domain id's wich should
 *   be deleted.
 */
function registrar_api_delete_multiple($dids) {
  entity_delete_multiple('registrar', $dids);
}

/**
 * class RegistrarEntity
 */
class RegistrarEntity extends Entity {
  
}

/**
 * class RegistrarController
 */
class RegistrarController extends EntityAPIController {
  
  public function create(array $values = array()) {
    //global $user;
    $values += array(
      'created' => REQUEST_TIME,
      'changed' => REQUEST_TIME,
    );
    return parent::create($values);
  }
}