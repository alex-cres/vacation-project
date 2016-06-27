<?php

/**
 * @file
 * The settings file that allows to changes settings regarding emails etc.
 */

require_once "event_calendar.list.inc";

/**
 * Menu callback: Event Calendar configuration.
 */
function event_calendar_settings($form, $form_state)
{
    if (module_exists('event_calendar_holi_counter') && !module_exists('core_table_cms')) {
	
        $form['event_calendar_custom_ticket']           = array(
            '#type' => 'fieldset',
            '#title' => t('Ticket Settings'),
            '#collapsible' => TRUE,
            '#collapsed' => FALSE,
            '#description' => t('Custom Ticket Part')
        );
        $form['event_calendar_custom_ticket']['text']   = array(
            '#type' => 'textfield',
            '#title' => t('Custom Ticket Extension'),
            '#default_value' => variable_get('ticket_event_calendar'),
            '#maxlength' => 30
        );
        $form['event_calendar_custom_ticket']['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Update'),
            '#submit' => array(
                'event_admin_custom_ticket_submit'
            )
            
        );
        
    }
    
    /*
    // Creating filter form.
    $form['event_calendar_email_reciepents'] = array(
    '#type' => 'fieldset',
    '#title' => t('Email reciepents'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('Select the roles who will recieve Email. Admin will recieve Email on creation of a new Event and Other users will recieve when Event is approved. Default super admin will recieve Email on creation of a new Event.'),
    );
    $roles = user_roles();
    unset($roles[DRUPAL_ANONYMOUS_RID]);
    unset($roles[DRUPAL_AUTHENTICATED_RID]);
    $form['event_calendar_email_reciepents']['event_calendar_email_reciepents_list'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Roles'),
    '#options' => $roles,
    '#default_value' => variable_get('event_calendar_email_reciepents_list', array()),
    );
    
    // Email settings for admin.
    $form['event_calendar_email_admin'] = array(
    '#type' => 'fieldset',
    '#title' => t('Email settings for admin'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Edit the e-mail messages sent to administrator/s. Available variables are: [site:name], [site:url], [node:author], [node:created], [node:title], [node:body], [node:url], [event_calendar:start_date], [event_calendar:end_date],[event_calendar:total_days],[event_calendar:working_days],[event_calendar:holi_days], [event_calendar:event_status].'),
    );
    $form['event_calendar_email_admin']['event_calendar_email_admin_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject for New Event'),
    '#default_value' => _event_calendar_email_text('admin_subject'),
    '#maxlength' => 180,
    );
    $form['event_calendar_email_admin']['event_calendar_email_admin_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body for New Event'),
    '#default_value' => _event_calendar_email_text('admin_body'),
    '#rows' => 15,
    );
    $form['event_calendar_email_admin']['event_calendar_email_admin_alter_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject for Modified Event'),
    '#default_value' => _event_calendar_email_text('admin_alter_subject'),
    '#maxlength' => 180,
    );
    $form['event_calendar_email_admin']['event_calendar_email_admin_alter_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body for Modified Event'),
    '#default_value' => _event_calendar_email_text('admin_alter_body'),
    '#rows' => 15,
    );
    
    
    // Email settings for users.
    $form['event_calendar_email_user'] = array(
    '#type' => 'fieldset',
    '#title' => t('Email settings for users'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Edit the e-mail messages sent to selected user roles. Available variables are: [site:name], [site:url], [node:author], [node:created], [node:title], [node:body], [node:url], [event_calendar:start_date], [event_calendar:end_date], [event_calendar:total_days],[event_calendar:working_days],[event_calendar:holi_days],[event_calendar:event_status].'),
    );
    $form['event_calendar_email_user']['event_calendar_email_user_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => _event_calendar_email_text('users_subject'),
    '#maxlength' => 180,
    );
    $form['event_calendar_email_user']['event_calendar_email_users_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#default_value' => _event_calendar_email_text('users_body'),
    '#rows' => 15,
    );
    */
    $form['event_calendar_admin']                         = array(
        '#type' => 'fieldset',
        '#title' => t('Code for Import'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#description' => t('The code for the calendar view')
        
    );
    $form['event_calendar_admin']['event_calendar_admin'] = array(
        '#type' => 'textarea',
        '#title' => t('Code'),
        '#default_value' => _code()
        
    );
    
    return $form;
}


function event_admin_custom_ticket_submit($from, $form_state)
{
    $op = $form_state['values']['text'];
    variable_set('ticket_event_calendar', $op);
    drupal_set_message("Extension set to \"" . $op . "\"");
    
}





//temporary import code

function _code()
{
    $text = file_get_contents(DRUPAL_ROOT . "/sites/all/modules/event_calendar/code_view.txt");
    
    return $text;
}