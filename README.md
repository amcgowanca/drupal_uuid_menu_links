## UUID Menu Links

Allows for entities (e.g. nodes) to be loaded and handled with their UUIDs provided by Drupal's UUID contributed module within the Drupal menu routing system.

#### Installation

* Download the module and place within the `sites/all/modules` directory.
* Using the `menu-object-load-override.patch` file found within the patches directory, patch Drupal core's `includes/menu.inc` file.
* Install the module either via the Administration or by using Drush, `drush en uuid_menu_links`.

#### The Problem

The problem that many Drupal developers run into when working with Features for menu links and default content (e.g. nodes) is ensuring that the menu links correspond to the default content (e.g. node) by default on installation without additional operations to be performed on the feature after render.

Using Drupal's "nodes" as a primary example, when a node is created it is assigned an identifier that is commonly known as the node id (nid). This identifier is used within any menu link that is created and stored within the `{menu_links}` database table. As a result, a node's menu link is saved within the `link_path` column as `node/[nid]`. When such content and menu links are used when building "profile" oriented distributions or sites using Drupal, exporting this data is not feasible due to the node identifier's not being portable. Specifically the node identifiers persist in the menu links that are exported and therefore do not allow for menu links to be truly exportable. With the UUID module installed, accessing nodes via `node/[uuid]` where `[uuid]` is the node's universal unique identifier is possible. However, UUID does not re-write menu links for all entities (e.g. nodes) when they are stored in the `{menu_links}` table and therefore the common problem many of us have exists.

Therefore, this module attempts to solve the problem described above by:

 * Re-write menu links for core contributed modules using the UUID of the entity instead of the entity's primary key.
 * Overwrites the common menu paths for loading entities to contain a custom menu load token to handle the loading of the entities.
 * Provides a patch to Drupal core (which is an installation requirement) that handles `menu_get_object()`'s first parameter of `$type = 'node'` to allow overriding its value so that the custom menu token can be used and compared correctly where `$type` equals `node`.

#### Why is Drupal core requiring a patch?

The patch that is provided within the patches directory of this project is used to introduce a "global" static value that is used within to override the `$type` parameter of the `menu_get_object()` function. The `$type` parameter has a default value of `node` and as a result, the Drupal core Node module relies heavily on this default value when invoking the `menu_get_object()` function. The override global static value is implemented using the `drupal_static()` function with the identifier of `menu_object_load_type_override`.

This newly introduced global static value is used for simply the purpose of overriding the `node` default value. The `$type` parameter's value is used to determine whether or not the menu router's load function was for a particular menu argument to be loaded. By default, the node module implements the routing path as `node/%node` and therefore the load function used for loading of nodes is `node_load()` when `%node` is used within a menu routing path. Due to menu links being re-written to contain the UUID and `node_load` not being aware the UUID as an identifier, a special load function was required to be implemented that attempts to load a node. This load function is made known to Drupal by altering the `node/%node` routing path via the implementation of `hook_menu_alter(&$menu)`. The `$menu['node/%node']` menu routing path is removed and replaced with `$menu['node/%uuid_menu_links_universal_node']` and therefore making Drupal aware of the newly implemented load function named `uuid_menu_links_universal_node_load` for automatically loading nodes.

Therefore as Drupal core's Node module heavily relies on the default value of the `$type` parameter in the `menu_get_object()` function, using the global static value, this module can override the default value (node) that the Drupal core's Node module heavily relies on allowing for correct evaluation of the line of code below when `menu_get_object()` is invoked with default parameter values.

```
if (isset($router_item['load_functions'][$position]) && !empty($router_item['map'][$position]) && $router_item['load_functions'][$position] == $type . '_load') {
```

Similarily this is also the case for taxonomy terms, in which Drupal core's taxonomy module passes to the `menu_get_object()` the `$type` paramater value of `taxonomy_term`. Perhaps there are more within Drupal core that I have yet to discover (or write here), but question is whether or not Drupal 7's current implementation of how `menu_get_object()` is actually accurate or implemented in the best possible way that will allow developers to overwrite menu items like `$menu['node/%node']` or `$menu['taxonomy/term/%taxonomy_term']` and allow the `$router_item['load_functions'][$position] == $type . '_load'` to be evaluated correctly.

### License

The UUID Menu Links is licensed under the [GNU General Public License](http://www.gnu.org/licenses/gpl-2.0.html) version 2.
