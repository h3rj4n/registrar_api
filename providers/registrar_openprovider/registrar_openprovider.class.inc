<?php

/*
 * class Registrar
 */

class RegistrarOpenProvider extends RegistrarApiProvider {
  
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
  public function check_domains($domain_list) {
    
  }

  /**
   * Returns an array of domains with the following structure:
   *
   * Array (
   *   [domaintest.com] => Array (
   *     [ID] => 1836
   *     [Name] => domaintest.com
   *     [User] => owner
   *     [Created] => 03/24/2009
   *     [Expires] => 03/24/2010
   *     [IsExpired] => false
   *   )
   * )
   * @todo integrate additional data?
   * @todo add optional parameters: limit, search pattern?
   */
  public function get_domains_list() {
    
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
  public function create_domain($domain_name, $years, $registrant, $tech, $admin, $auxbilling, $nameservers = NULL) {
    
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
  public function renew_domain($domain, $years) {
    
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
  public function set_contacts($domain, $years, $registrant, $tech, $admin, $auxbilling) {
    
  }
  
  /**
   * Get the contact information for a domain
   *
   * @param $domain
   *   The name of the domain to retrieve the contact information for
   *
   * @return array containing the domain contact information
   */
  public function get_contacts($domain) {
    
  }
  
  public function settingsForm(&$form, &$form_state) {
    
    $form['registrar_openprovider'] = array(
      '#type' => 'fieldset',
      '#title' => t('OpenProvider Registrar Settings'),
    );
  
    $form['registrar_openprovider']['registrar_openprovider_uri'] = array(
      '#type' => 'textfield',
      '#title' => t('OpenProvider Service URL'),
      '#default_value' => variable_get('registrar_openprovider_uri', 'https://api.openprovider.eu/'),
      '#description' => t("i.e. https://api.openprovider.eu/"),
      '#required' => TRUE,
    );
  
    $form['registrar_openprovider']['registrar_openprovider_api_user'] = array(
      '#type' => 'textfield',
      '#title' => t('OpenProvider API User'),
      '#default_value' => variable_get('registrar_openprovider_api_user', ''),
      '#description' => t("This is usually your regular openprovider username"),
      '#required' => TRUE,
    );
  
    $form['registrar_openprovider']['registrar_openprovider_api_password'] = array(
      '#type' => 'textfield',
      '#title' => t('OpenProvider Password'),
      '#default_value' => variable_get('registrar_openprovider_api_password', ''),
      '#required' => TRUE,
    );
    
    return $form;
  }
  
  public function settingsFormValidate($form, $form_state) {
    // TODO
  }
}
