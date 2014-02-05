## UUID Menu Links

Allows for entities (e.g. nodes) to be loaded and handled with their UUIDs provided by Drupal's UUID contributed module within the Drupal menu routing system.

The problem that many Drupal developers have when working with Features as well as "default" content, whether this is landing pages to actual basic content, is when such content is featurized as well as having menu links to the corresponding content. Let's use Drupal's "nodes" for example. When a node is created it is assigned an identifier that is commonly known as the node id (nid). This identifier is used within any menu link that is created and stored within the `{menu_links}` table. For example, a node's menu link is saved with the `link_path` of `node/[nid]` where is the node identifier. When such content and menu links are used when building "profile" oriented distributions or sites using Drupal, exporting content is not feasible without the use of UUID and UUID Features contrib modules due to their node identifier's not being portable. However, node identifiers persist in the menu links that are exported and therefore do not allow for menu links to be truly exportable. With the UUID module installed, accessing nodes via `node/[uuid]` where `[uuid]` is the node's universal unique identifier is possible. However, UUID does not re-write menu links for all entities (e.g. nodes) when they are stored in the `{menu_links}` table.

Therefore, this module attempts to solve the problem above by:

 * Re-write menu links for core contributed modules using the UUID of the entity instead of the entity's primary key.
 * Overwrites the common menu paths for loading entities to contain a custom menu load token to handle the loading of the entities.
 * Provides a patch to Drupal core (which is an installation requirement) that handles `menu_get_object()`'s first parameter of `$type = 'node'` to allow overriding its value so that the custom menu token can be used and compared correctly where `$type` equals `node`.

#### Installation

* Download the module and place within the `sites/all/modules` directory.
* Using the `menu-object-load-override.patch` file found within the patches directory, patch Drupal core's `includes/menu.inc` file.
* Install the module either via the Administration or by using Drush, `drush en uuid_menu_links`.

#### Why is Drupal core requiring a patch?

TODO: Write explanation here as to why.

### License

The UUID Menu Links is licensed under the [GNU General Public License](http://www.gnu.org/licenses/gpl-2.0.html) version 2.
