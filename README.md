## UUID Menu Links

Allows for entities (e.g. nodes) to be loaded and handled with their UUIDs provided by Drupal's UUID contributed module within the Drupal menu routing system.

TODO: Write and explain the problem that this module solves.

#### Installation

* Download the module and place within the `sites/all/modules` directory.
* Using the `menu-object-load-override.patch` file found within the patches directory, patch Drupal core's `includes/menu.inc` file.
* Install the module either via the Administration or by using Drush, `drush en uuid_menu_links`.

#### Why is Drupal core requiring a patch?

TODO: Write explanation here as to why.

### License

The UUID Menu Links is licensed under the [GNU General Public License](http://www.gnu.org/licenses/gpl-2.0.html) version 2.
