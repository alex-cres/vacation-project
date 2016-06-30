MODULE
------
Event Calendar

Contents of files:
------------------

  * Installation
  * Configuration


REQUIREMENTS
------------
Drupal 7.0


DESCRIPTION/FEATURES
--------------------

  The Event Calendar module allows users to Add/Edit/View events in pop-up while
  clicking on a box in an “event calendar view”. This module uses the calendar
  display format of Calendar module and gives its own content type and views.
  It manages colors of events based on their status (taxonomy field) that can
  be set on configuration page.
 
  This module also gives an Admin UI to change status (approval/denied) of
  events and a mailing system.

 
Features
----------


 *  A mailing integration for approval of event.
 *  In-place Add/Edit/View of events in pop-up.
 *  Different colors of events based on their status.
 *  Vacation creation.
 *  Counter for used days.
 *  Submission and Cancel.

 
 
Benefits
----------


 * No need to add content type.
 * Easy to add events (if a user permits).
 * Events status can be easily identified with the color of events.

 
Dependency
----------
  As it uses calendar format for the creation of “event calendar view” so it
  depends on the Calendar module. If you want to use dates in pop-up then it
  requires Date-pop also.
 
  * Calendar => http://drupal.org/project/calendar .
  * Date => http://drupal.org/project/date .
  * Date Popup.
  * Relation.
  * PHP.
  






Installation:
-------------
1. Copy Event Calendar folder to modules (usually 'sites/all/modules')
   directory.
2. At the 'admin/modules' page, enable the IP address manager module.


Configuration:
--------------
At the 'admin/config/date/event-settings' page enable Event Calendar
for event settings.


CREDITS
--------



* This module was created by OSSCube Solutions Pvt Ltd. 
* Guided by Bhupendra Singh. 
* Developed by Radhey Shyam and Navneet Kumar. 
* It was further continued by Alexandre Realinho.
