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

class RegistrarEntity extends Entity {
  
}

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