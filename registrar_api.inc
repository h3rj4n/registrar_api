<?php
// $Id$
/**
 * @file
 * Provides all of the Registrar API calls.
 */

/**
 * Check the availability status on a list of domains.
 *
 * @param $domain_list
 *   An array containing a list of domains to test availability on
 *
 * @return multi-dimensional array key'd by domain name, for example:
 *   Array (
 *     [domain1.com] => Array (
 *       [Domain] => domain1.com,
 *       [Available] => false,
 *     )
 *   )
 */
function registrar_api_check_domains($domain_list) {
  return _registrar_api_method('check_domains', implode(", ", $domain_list));
}

/**
 * Check the availability status on the specified domain.
 *
 * @param $domain
 *   The domain name to check availability on
 *
 * @return TRUE if domain is available, FALSE otherwise
 */
function registrar_api_check_domain($domain) {
  $domain_results = registrar_api_check_domains(array($domain));
  if ($domain_results[$domain]['Available'] == "true") {
      return TRUE;
  }
  return FALSE;
}

/**
 * Register the given domain name
 *
 * @param $domain_name
 *   The domain name to register
 * @param $years
 *   The number of years to register the domain for
 * @param $registrant
 *   An array containing all of the registrant contact details
 * @param $tech
 *   An array containing all of the tech contact details
 * @param $admin
 *   An array containing all of the admin contact details
 * @param $auxbilling
 *   An array containing all of the billing details
 * @param $nameservers
 *   The nameservers to use for the domain
 *
 * return array containing the registered domain information
 */
function registrar_api_create_domain($domain_name, $years, $registrant,
  $tech, $admin, $auxbilling, $nameservers = NULL) {

  $contact = _registrar_api_convert_contacts(
    $registrant, $tech, $admin, $auxbilling);

  return _registrar_api_method('create_domain',
    $domain_name, $years, $contact['registrant'], $contact['tech'],
    $contact['admin'], $contact['auxbilling'], $nameservers
  );
}

/**
 * Check the availability status on the specified domain.
 *
 * @param $domain
 *   The domain name to renew
 *
 * @param $years
 *   The number of years to renew the domain for
 *
 * @return array containing renewal information
 */
function registrar_api_renew_domain($domain, $years) {
  return _registrar_api_method('renew_domain', $domain, $years);
}

/**
 * Set the contact information on the domain
 *
 * @param $domain
 *   The domain to be updated
 * @param $registrant
 *   An array containing all of the registrant contact details
 * @param $tech
 *   An array containing all of the tech contact details
 * @param $admin
 *   An array containing all of the admin contact details
 * @param $auxbilling
 *   An array containing all of the billing details
 *
 * @return none
 */
function registrar_api_set_contacts($domain, $years, $registrant,
  $tech, $admin, $auxbilling) {

  $contact = _registrar_api_convert_contacts(
    $registrant, $tech, $admin, $auxbilling);
  _registrar_api_method('set_contacts', $domain, $contact['registrant'],
    $contact['tech'], $contact['admin'], $contact['auxbilling']);
}

/**
 * @param $domain
 *   The name of the domain to retrieve the contact information for
 *
 * @return array containing the domain contact information
 */
function registrar_api_get_contacts($domain) {
  $contacts = _registrar_api_method('get_contacts', $domain);
  return _registrar_api_convert_contacts_reverse(
    $contacts['Registrant'], $contacts['Tech'],
    $contacts['Admin'], $contacts['AuxBilling']);
}

/**** Helper functions ****/

function _registrar_api_method() {
  $args = func_get_args();
  $method = array_shift($args);

  $registrar = variable_get('registrar_api_registrar', '');
  $method = $registrar . '_' . $method;

  return call_user_func_array($method, $args);
}

/**
 * Converts the keys in each contact array to it's CamelCase equivelant.
 * i.e. first_name => FirstName
 *
 * This is the format that namecheap needs the data to be in.
 */
function _registrar_api_convert_contacts($registrant, $tech, $admin,
  $auxbilling) {

  // Convert the contact details' keys to CamelCase
  $contact_copy = array(
    'registrant' => array(), 'tech' => array(),
    'admin' => array(), 'auxbilling' => array());

  foreach ($contact_copy as $type => $values) {
    foreach ($$type as $key => $value) {
      $newkey = str_replace('_', ' ', $key);
      $newkey = str_replace(' ', '', ucwords($newkey));
      $contact_copy[$type][$newkey] = $value;
    }
  }

  return $contact_copy;
}

function _registrar_api_convert_contacts_reverse(
  $registrant, $tech, $admin, $aux_billing) {
  // Convert the contact details' keys from CamelCase to lower with _'s
  $field_map = array(
    'OrganizationName' => 'organization_name',
    'JobTitle' => 'job_title',
    'FirstName' => 'first_name',
    'LastName' => 'last_name',
    'Address1' => 'address1',
    'Address2' => 'address2',
    'City' => 'city',
    'StateProvince' => 'state_province',
    'StateProvinceChoice' => 'state_province_choice',
    'PostalCode' => 'postal_code',
    'Country' => 'country',
    'Phone' => 'phone',
    'Fax' => 'fax',
    'EmailAddress' => 'email_address',
    'PhoneExt' => 'phone_ext'
  );
  $contact_copy = array('registrant' => array(), 'tech' => array(),
                        'admin' => array(), 'aux_billing' => array());
  foreach ($contact_copy as $type => $values) {
    foreach ($$type as $key => $value) {
      $newkey = $field_map[$key];
      $contact_copy[$type][$newkey] = $value;
    }
  }

  return $contact_copy;
}
