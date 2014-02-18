<?php
/**
 * @file
 */

/**
 * Hook to allow other modules to run synchronization tasks.
 */
function hook_uuid_menu_links_sync() {
  variable_set('uuid_menu_links_last_sync', REQUEST_TIME);
}

/**
 * Allows modules to define new load hooks for menu routing paths.
 *
 * @return array
 *   Returns the information about new load hooks.
 */
function hook_uuid_menu_links_info() {
  return array(
    'node' => array(
      'original_load_hook' => 'node_load',
      'load_hook' => 'uuid_menu_links_universal_node_load',
      'menu_path' => array(
        'prefix' => 'node',
        'ignored_tokens' => array('%node_type'),
      ),
    ),
    'user' => array(
      'original_load_hook' => 'user_load',
      'load_hook' => 'uuid_menu_links_universal_user_load',
      'menu_path' => array(
        'prefix' => 'user',
        'ignored_paths' => array('user/register', 'user/login', 'user/logout', 'user/password'),
      ),
    ),
  );
}

/**
 * Allows modules to alter the uuid_menu_links information.
 *
 * @param array $info
 *   The alterable information defined by hook_uuid_menu_links().
 */
function hook_uuid_menu_links_info_alter(array &$info) {
  // Removes the `node` type from being available as a UUID Menu Link compatible.
  unset($info['node']);
}
