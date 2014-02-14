## UUID Menu Links

Allows for entities (e.g. nodes) to be loaded and handled with their UUIDs provided by Drupal's UUID contributed module within the Drupal menu routing system.

### Installation

#### Common Installation

* Download the module and place within the `sites/all/modules` directory.
* Using the `menu_get_object-load-type-alter.patch` file found within the patches directory, patch Drupal core's `includes/menu.inc` file.
* Install the module either via the Administration or by using Drush, `drush en uuid_menu_links`.

#### As part of Drush Make

This project can be downloaded and used within a Drush make file like any other contributed module. The difference is that this module exists on GitHub instead of Drupal.org. Within your `drupal-org.make`, the UUID Menu Links module can be defined as a project. This is done with:

```
projects[uuid_menu_links][type] = "module"
projects[uuid_menu_links][download][type] = "git"
projects[uuid_menu_links][download][url] = "git@github.com:amcgowanca/uuid_menu_links.git"
projects[uuid_menu_links][download][tag] = "7.x-0.5"
```

Furthermore, if you are using this module with Drush Make files, it is recommended that the Drupal core is also patched using a one line download to the available patch file. This is done by adding the line below immediately after where the Drupal core project is defined:

```
projects[drupal][patch][] = "https://raw2.github.com/amcgowanca/drupal_uuid_menu_links/7.x-0.5/patches/menu_get_object-load-type-alter.patch"
```

**Notes:**

* This module is downloaded using Git.
* This module will be downloaded the tagged release version 7.x-0.5.
* The patch file provided will be downloaded from the 7.x-0.5 tag release.


### The Problem

The problem that many Drupal developers run into when working with Features for menu links and default content (e.g. nodes) is ensuring that the menu links correspond to the default content (e.g. node) by default on installation without additional operations to be performed on the feature after render.

Using Drupal's "nodes" as a primary example, when a node is created it is assigned an identifier that is commonly known as the node id (nid). This identifier is used within any menu link that is created and stored within the `{menu_links}` database table. As a result, a node's menu link is saved within the `link_path` column as `node/[nid]`. When such content and menu links are used when building "profile" oriented distributions or sites using Drupal, exporting this data is not feasible due to the node identifier's not being portable. Specifically the node identifiers persist in the menu links that are exported and therefore do not allow for menu links to be truly exportable. With the UUID module installed, accessing nodes via `node/[uuid]` where `[uuid]` is the node's universal unique identifier is possible. However, UUID does not re-write menu links for all entities (e.g. nodes) when they are stored in the `{menu_links}` table and therefore the common problem many of us have exists.

Therefore, this module attempts to solve the problem described above by:

 * Re-write menu links for core contributed modules using the UUID of the entity instead of the entity's primary key.
 * Overwrites the common menu paths for loading entities to contain a custom menu load token to handle the loading of the entities.
 * Provides a patch to Drupal core (which is an installation requirement) that handles `menu_get_object()`'s first parameter of `$type = 'node'` to allow overriding its value so that the custom menu token can be used and compared correctly where `$type` equals `node`.

### Why is Drupal core requiring a patch?

The patch that is provided within the patches directory of this project is used to introduce a new alter hook named `menu_get_object_load_type` that is used to alter the `$type` parameter of the `menu_get_object()` function. The `$type` parameter has a default value of `node` and as a result, the Drupal core Node module relies heavily on this default value when invoking the `menu_get_object()` function. The override global static value is implemented using the `drupal_static()` function with the identifier of `menu_object_load_type_override`.

This newly introduced alter hook is used for simply the purpose of overriding (altering) the value of `$type` where needed. The `$type` parameter's value is used to determine whether or not the menu router's load function was for a particular menu argument to be loaded. By default, the node module implements the routing path as `node/%node` and therefore the load function used for loading of nodes is `node_load()` when `%node` is used within a menu routing path. Due to menu links being re-written to contain the UUID and `node_load` not being aware the UUID as an identifier, a special load function was required to be implemented that attempts to load a node. This load function is made known to Drupal by altering the `node/%node` routing path via the implementation of `hook_menu_alter(&$menu)`. The `$menu['node/%node']` menu routing path is removed and replaced with `$menu['node/%uuid_menu_links_universal_node']` and therefore making Drupal aware of the newly implemented load function named `uuid_menu_links_universal_node_load` for automatically loading nodes.

Therefore as Drupal core's Node module heavily relies on the default value of the `$type` parameter in the `menu_get_object()` function, using the global static value, this module can override the default value (node) that the Drupal core's Node module heavily relies on allowing for correct evaluation of the line of code below when `menu_get_object()` is invoked with default parameter values.

```
if (isset($router_item['load_functions'][$position]) && !empty($router_item['map'][$position]) && $router_item['load_functions'][$position] == $type . '_load') {
```

Similarily this is also the case for taxonomy terms, in which Drupal core's taxonomy module passes to the `menu_get_object()` the `$type` paramater value of `taxonomy_term`. Perhaps there are more within Drupal core that I have yet to discover (or write here), but question is whether or not Drupal 7's current implementation of how `menu_get_object()` is actually accurate or implemented in the best possible way that will allow developers to overwrite menu items like `$menu['node/%node']` or `$menu['taxonomy/term/%taxonomy_term']` and allow the `$router_item['load_functions'][$position] == $type . '_load'` to be evaluated correctly.

### The Patch and newly introduced Hook

The patch provided introduces a new alter hook defined as such:

```
function hook_menu_get_object_load_type_alter(&$type, $original_type) {
  switch ($original_type) {
    case 'node':
      if ('node' == arg(0)) {
        // Sets the custom menu routing token that is found within the
        // the menu by overriding the `node/%node` with `node/%mymodule_node_new`.
        // Therefore the module implementing this override in the menu routing should
        // also implement the function `mymodule_node_new_load` in which is used for
        // "autoloading" the node object for the page callback.
        $type = 'mymodule_node_new';
      }
      break;
  }
}
```

### UUID Menu Links and Pathauto

It is not untypical for many developers to leverage the power of Drupal 7's contributed module [Pathauto](http://drupal.org/project/pathauto) as part of a site build. Using the UUID Menu Links module in conjunction with Pathauto you will soon see that there is a problem. This is due to UUID Menu Links re-writing the source path of URL aliases and as a result, when Pathauto attempts to locate existing paths using the standard non-portable source path, Pathauto will actually create a duplicate entry. While Pathauto provides the `hook_pathauto_alias_alter()` hook which allows for the source to be altered via `$context['source']` (the second parameter of the alter hook, passed by reference) it is invoked after the `_pathauto_existing_alias_data()` function that uses the `$source` of an alias to find an existing path. Due to this, a simple patch is required to allow for the `$source` parameter of `_pathauto_existing_alias_data()` to be altered prior to the database query execution.

Available within the patches directory is the [pathauto-existing_alias_source_alter-hook.patch](https://github.com/amcgowanca/drupal_uuid_menu_links/blob/7.x-1.x/patches/pathauto-existing_alias_source_alter-hook.patch) that introduces a new alter hook named `pathauto_existing_alias_source` implemented as `hook_pathauto_existing_alias_source_alter()`. This hook is implemented within the UUID Menu Links module so that source paths with non-portable identifiers are transfered into those with UUID (e.g. `node/[nid]` => `node/[uuid]`).

### UUID Menu Links and Entity

The Drupal 7's contributed module [Entity API](http://drupal.org/project/entity) is typically used in all Drupal 7 site builds. However, the Entity module is not aware of the UUID module when they are installed side-by-side on the same project. When using the Entity Wrappers for loading an entity based on the UUID, the Entity Wrapper throws an Exception during the validation of the value using to retrieve the entity. As a result, a patch is required for the function `entity_property_verify_data_type()`.

Available within the patches directory is the [entity-support_for_uuid_as_types.patch](https://github.com/amcgowanca/drupal_uuid_menu_links/blob/7.x-1.x/patches/entity-support_for_uuid_as_types.patch) that allows for the validation of the UUID if the entity contains the UUID property key.

### Synchronize non-portables

Non-portable menu links and path alias sources can be synchronized (updated with UUIDs) by invoking the drush command `drush uuid-menu-links-sync` or by invoking the function `uuid_menu_links_sync()`.

### License

The UUID Menu Links is licensed under the [GNU General Public License](http://www.gnu.org/licenses/gpl-2.0.html) version 2.
