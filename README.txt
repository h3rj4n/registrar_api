/* $Id */

-- SUMMARY --

The registrar_api provides an interface to 3rd party domain registrars to
allow registration, renewal, and domain management through a Drupal
installation.

-- Registrar APIs --

Currently there is only support for Namecheap.com's (http://www.namecheap.com)
API. If you want support for another registrar, either implement a new .inc
file (using namecheap.inc as an example) and submit a patch or let me know
about the registrar and I'll look into it.

-- REQUIREMENTS --

Each registrar has different requirements in order to use their API and you'll
likely need to create an account with them and obtain an API key at the very
least.

Namecheap.com - Details at: http://developer.namecheap.com/docs/doku.php?id=overview:3.api.access

-- INSTALLATION --

 * Install as usual.

-- CONFIGURATION --
 * Configure user permissions in Administer >> User management >>
   Permissions >> regisrar_api module:
   
   - register domains
    Users in this role can register domains
    
   - edit own domains
    Users can edit their own domain information
    
   - manage domains
    Users in this role can manage anyone's domain settings and renewals.
 
 * Configure the registrars API settings in Administer >> Settings >> Registrar API

-- CREATING DOMAINS --
 * Before creating a domain you should fill in the generic contact information
   on the user profile form My Account >> Edit >> Registrar Contact Info

 * The registrar_api front end adds a new node type called 'domain'. This will,
   by default, pull in the data entered on the users profile form and can be
   changed at the time of registration.
   
-- CONTACT --

Current maintainer:
 * Scott Hadfield (hadsie) - http://drupal.org/user/67093,
                             http://scotthadfield.ca
