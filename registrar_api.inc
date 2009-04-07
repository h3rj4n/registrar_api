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
  return namecheap_domains_check($domain_list);
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
  //print_r($contact_copy);

  return namecheap_domains_create(
    $domain_name, $years,
    $contact_copy['registrant'], $contact_copy['tech'],
    $contact_copy['admin'], $contact_copy['auxbilling'],
    $nameservers
  );

}