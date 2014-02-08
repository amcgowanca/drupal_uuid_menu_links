<?php
/**
 * @file
 */

/**
 * Implements hook_drush_command().
 */
function uuid_menu_links_drush_command() {
  $commands = array();
  $commands['uuid-menu-links-sync'] = array(
    'description' => 'Synchronize all menu links and path alias sources with UUIDs.',
  );
  return $commands;
}

/**
 * Synchronizes all menu links and path alias sources.
 */
function drush_uuid_menu_links_sync() {
  uuid_menu_links_sync();
  drush_log(dt('Synchronized all menu links and path alias sources.'), 'ok');
}
