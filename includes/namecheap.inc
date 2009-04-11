<?php
// $Id$

/**
 * @file
 * Namecheap API utility functions.
 */

function namecheap_admin_form() {
  $form = array();

  $form['namecheap'] = array(
    '#type' => 'fieldset',
    '#title' => t('Namecheap Registrar Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
   
  $form['namecheap']['namecheap_uri'] = array(
    '#type' => 'textfield',
    '#title' => t('Namecheap Service URL'),
    '#default_value' => variable_get('namecheap_uri', ''),
    '#description' => t("i.e. https://api.sandbox.namecheap.com/xml.response"),
    '#required' => TRUE,
  );

  $form['namecheap']['namecheap_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Namecheap API key'),
    '#default_value' => variable_get('namecheap_api_key', ''),
    '#required' => TRUE,
  );
  
  $form['namecheap']['namecheap_api_user'] = array(
    '#type' => 'textfield',
    '#title' => t('Namecheap API User'),
    '#default_value' => variable_get('namecheap_api_user', ''),
    '#description' => t("This is usually your regular namecheap username"),
    '#required' => TRUE,
  );
  return $form;
}

/* Domain API Functions */

/**
 * Input: $domain_list = "domain1.com,availabledomain.com,err.tld"
 * Output:
 * Array (
 *   [domain1.com] => Array (
 *     [Domain] => domain1.com
 *     [Available] => false
 *     [ErrorNo] => 
 *     [Description] => 
 *   )
 *   [availabledomain.com] => Array (
 *     [Domain] => availabledomain.com
 *     [Available] => true
 *     [ErrorNo] => 
 *     [Description] => 
 *   )
 *   [err.tld] => Array (
 *     [Domain] => err.tld
 *     [Available] => error
 *     [ErrorNo] => 703
 *     [Description] => Timeout while waiting for server response
 *   )
 * )
 */
function namecheap_domains_check($domain_list) {

  // TODO: Retry on failed attempts
  
  $request = _namecheap_init_request('namecheap.domains.check');
  $request['DomainList'] = $domain_list;
  $response = _namecheap_request($request);

  $sxml = new SimpleXMLElement($response);
  $attributes = array('Domain', 'Available', 'ErrorNo', 'Description');
  $domains = _namecheap_xmlattr2array(
    $response,
    $sxml->CommandResponse->DomainCheckResult,
    $attributes,
    'Domain'
  );
  
  return ($domains);
}

function namecheap_domains_create($domain_name, $years, $registrant, $tech, $admin, $auxbilling, $nameservers = NULL) {
  // TODO: Add promotion code, and extended attributes for .us, .ca, etc.
  // TODO: Test error handling (already registered domain, 0 years,
  // nameservers, etc.)

  $request = _namecheap_init_request('namecheap.domains.create');
  $request['DomainName'] = $domain_name;
  $request['Years'] = $years;

  foreach ($registrant as $field => $value) {
    $request['Registrant' . $field] = $value;
  }
  foreach ($tech as $field => $value) {
    $request['Tech' . $field] = $value;
  }
  foreach ($admin as $field => $value) {
    $request['Admin' . $field] = $value;
  }
  foreach ($auxbilling as $field => $value) {
    $request['AuxBilling' . $field] = $value;
  }

  // TODO: Validate nameserver format
  if ($nameservers) {
    $request['Nameservers'] = $nameservers;
  }
  
  $response = _namecheap_request($request);
  $sxml = new SimpleXMLElement($response);
  
  $attributes = array('Domain', 'DomainID', 'Registered', 'OrderId',
                      'TransactionId', 'ChargedAmount');
  $domain = _namecheap_xmlattr2array(
    $response,
    $sxml->CommandResponse->DomainCreateResult,
    $attributes
  );

  return $domain[0];
}

function namecheap_domains_getcontacts($domain_name) {
  // TODO: Handle requests when the domain isn't in this users account
  // Test this with all 4 different contact types have different data
  
  $request = _namecheap_init_request('namecheap.domains.getContacts');
  $request['DomainName'] = $domain_name;
  $response = _namecheap_request($request);
  $sxml = new SimpleXMLElement($response);

  // Get the Domain details
  $attributes = array('Domain', 'sld', 'tld', 'domainnameid');
  $domain = _namecheap_xmlattr2array(
    $response,
    $sxml->CommandResponse->DomainContactsResult,
    $attributes
  );
  $domain = $domain[0];

  // Get the Registrant/Tech/Admin/AuxBilling Information
  $contacts = array(
    'Registrant' => array(),
    'Tech' => array(),
    'Admin' => array(),
    'AuxBilling' => array(),
  );
  
  $fields = array('OrganizationName', 'JobTitle', 'FirstName', 'LastName', 'Address1', 'Address2', 'City', 'StateProvince', 'StateProvinceChoice', 'PostalCode', 'Country', 'Phone', 'Fax', 'EmailAddress', 'PhoneExt');

  foreach ($contacts as $type => $data) {
    foreach ($fields as $field) {
      $contacts[$type][$field] = (string) $sxml->CommandResponse->DomainContactsResult->$type->$field;
    }
  }

  $contacts['domain'] = $domain;
  return ($contacts);
}

/**
 * Returns an array of domains with the following structure:
 *
 * Array (
 *   [domaintest.com] => Array (
 *     [ID] => 1836
 *     [Name] => domain-name.com
 *     [User] => owner
 *     [Created] => 03/24/2009
 *     [Expires] => 03/24/2010
 *     [IsExpired] => false
 *   )
 * ) 
 *
 */
function namecheap_domains_getlist() {
  // TODO: Handle the case when there is no domains to list
  
  $request = _namecheap_init_request('namecheap.domains.getList');
  $response = _namecheap_request($request);

  $sxml = new SimpleXMLElement($response);
  $attributes = array('ID', 'Name', 'User', 'Created', 'Expires', 'IsExpired');
  $domains = _namecheap_xmlattr2array(
    $response,
    $sxml->CommandResponse->DomainGetListResult->Domain,
    $attributes,
    'Name'
  );
  
  return ($domains);
}

function namecheap_domains_getregistrarlock() {
  /* Placeholder */
}

function namecheap_domains_reactivate() {
  /* Placeholder */
}

function namecheap_domains_renew($domain_name, $years) {
  // TODO: Add promotion code field
  
  $request = _namecheap_init_request('namecheap.domains.renew');
  $request['DomainName'] = $domain_name;
  $request['Years'] = $years;

  $response = _namecheap_request($request);

  $sxml = new SimpleXMLElement($response);
  $attributes = array('DomainName',  'DomainID',  'Renew', 'OrderID',
                      'TransactionID', 'ChargedAmount');
  $domain = _namecheap_xmlattr2array(
    $response,
    $sxml->CommandResponse->DomainRenewResult,
    $attributes
  );
  
  return ($domain[0]);

}

function namecheap_domains_setcontacts($domain_name, $registrant, $tech, $admin, $auxbilling) {
  // TODO: Implement extended attributes required for some cctlds

  $request = _namecheap_init_request('namecheap.domains.setcontacts');
  $request['DomainName'] = $domain_name;

  foreach ($registrant as $field => $value) {
    $request['Registrant' . $field] = $value;
  }
  foreach ($tech as $field => $value) {
    $request['Tech' . $field] = $value;
  }
  foreach ($admin as $field => $value) {
    $request['Admin' . $field] = $value;
  }
  foreach ($auxbilling as $field => $value) {
    $request['AuxBilling' . $field] = $value;
  }

  $response = _namecheap_request($request);
  $sxml = new SimpleXMLElement($response);

  $attributes = array('Domain', 'IsSuccess');
  $domain = _namecheap_xmlattr2array(
    $response,
    $sxml->CommandResponse->DomainSetContactResult,
    $attributes
  );

  return $domain[0];
}

function namecheap_domains_setregistrarlock() {
  /* Placeholder */
}


/* Utility Functions */

function _namecheap_init_request($command, $username = NULL) {
  if (!$username) {
    $username = variable_get('namecheap_api_user', '');
  }
  $request = array(
    'ApiUser' => variable_get('namecheap_api_user', ''),
    'ApiKey' => variable_get('namecheap_api_key', ''),
    'UserName' => $username,
    'ClientIP' => $_SERVER['REMOTE_ADDR'],
    'command' => $command,
  );
  return $request;
}

function _namecheap_request($request) {
  // TODO: Handle request timeouts and other failures

  $request_arr = array();
  foreach ($request as $field => $value) {
    $request_arr[] .= urlencode($field) . "=" . urlencode($value);
  }
  $request_str = variable_get('namecheap_uri', '') . "?" . implode("&", $request_arr);

  $f = @fopen($request_str, "r");
  $response = stream_get_contents($f);
  fclose($f);
  
  return $response;
}

/**
 *
 * Builds and array out of a list of xml elements and their attributes.
 * Doesn't use any data that may be in the element, attributes only.
 *
 * $response - the xml response string
 * $attributes - the attributes who's values should be returned
 * $index - which attribute to index the returned array with, if not used then
 *          it will default to a numeric index.
 *
 */
function _namecheap_xmlattr2array($response, $sxml_obj, $attributes, $index = NULL) {
  $data = array();
  
  foreach ($sxml_obj as $xrecord) {
    $record = array();
    foreach ($attributes as $attribute) {
      $record[$attribute] = (string) $xrecord[$attribute];
    }
    if (!$index) {
      $data[] = $record;
    }
    else {
      $data[$record[$index]] = $record;
    }
  }
  
  return ($data);

}


