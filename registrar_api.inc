<?php

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
function registrar_api_domains_check($domain_list) {
  return namecheap_domains_check(implode(", ", $domain_list));
}

/**
 * Check the availability status on the specified domain.
 *
 * @param $domain
 *   The domain name to check availability on
 *
 * @return TRUE if domain is available, FALSE otherwise
 */
function registrar_api_domain_check($domain) {
  $domain = registrar_api_domains_check(array($domain));
  return $domain['Available'];
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
function registrar_api_domains_create($domain_name, $years, $registrant, $tech, $admin, $auxbilling, $nameservers = NULL) {

  $contact = _registrar_api_convert_contacts($registrant, $tech, $admin,
                                             $auxbilling);
  return namecheap_domains_create(
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
function registrar_api_domains_renew($domain, $years) {
  return namecheap_domains_renew($domain, $year);
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
function registrar_api_domains_setcontacts($domain, $years, $registrant,
                                           $tech, $admin, $auxbilling) {
  $contact = _registrar_api_convert_contacts($registrant, $tech, $admin,
                                             $auxbilling);
  namecheap_domains_setcontacts($domain, $contact['registrant'],
                                $contact['tech'], $contact['admin'],
                                $contact['auxbilling']);
}


/**** Helper functions ****/
function _registrar_api_convert_contacts($registrant, $tech, $admin,
                                         $auxbilling) {
  // Convert the contact details' keys to CamelCase
  $contact_copy = array('registrant' => array(), 'tech' => array(),
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
