## URL Argument / HTTP-API and MQTT JSON Parameters:
|Arg   | Description                                                                     |
|------|---------------------------------------------------------------------------------|
|`MSG` | Message to display on dot matrix                                                |
|`REP` | Number of times the message scrolls horizontally across the dot matrix          |
|`BUZ` | Number of times the buzzer makes a sound (chirps) in repeated succession        |
|`DEL` | Delay in millisecond for each scrolling step (speed of scrolling message)       |
|`BRI` | Brightness of LED display (values ranging from 0 lowest to 15 highest)          |
|`ASC` | ASCII coversion to enable correct translation of UTF8 Extended ASCII Characters |

From version v2022.08.14, to send a message, you can send only some parameters, the other will take from the device default (currently hardcoded).

**Note:** The MSG parameter needs to be present to send a message, omitting the MSG parameter or sending no parameters will result in display termination of current message (end of message scrolling).

A couple of examples for sending JSON via MQTT or HTTP API.

### Example 1:

`{"MSG":"Test"}`

this is equivalent to sending a message to MQTT to a non /json ending topic, every other parameter will use default parameters values currently hardcoded in 01_Shared.h.

The following hardcoded defaults that will be use are:

```
REP 10
BUZ 10
DEL 35
BRI 7
ASC 1
```

### Example 2:
```
{
  "MSG":"Test",
  "BRI":"0"
}
```
This will send a Test message and display it at the lowest possible brightness.
The following hardcoded default parameters will be use:
```
REP 10
BUZ 10
DEL 35
ASC 1
```

## MQTT Topic Publishing/Subscribing

If you enter the following Topic Prefix "rdadotmatrix/generic" as part of your MQTT config, the following log message can be seen from console if "#define DEBUG 1" is defined:
```
Publishing to topic hostname/status: connected (this currently has no use)
Subscribe to topic: root_topic
Subscribe to topic: root_topic/json 
Subscribe to topic: root_topic/topic
Subscribe to topic: root_topic/topic/json
Subscribe to topic: hostname
Subscribe to topic: hostname/json
```

#### Example:
```
Restoring MQTT connection...
ESP-MSG-ABCDEF connected to MQTT Server: 192.168.1.100:1883
Publishing to topic ESP-MSG-ABCDEF/status: connected
Subscribe to topic: rdadotmatrix
Subscribe to topic: rdadotmatrix/json
Subscribe to topic: rdadotmatrix/generic
Subscribe to topic: rdadotmatrix/generic/json
Subscribe to topic: ESP-MSG-ABCDEF
Subscribe to topic: ESP-MSG-ABCDEF/json
```

### Note:

1. Any message published to a subscribed topic ending with /json will require a json message with any number of parameters passed above (MSG is mandatory to display a message).

2. Any message published to a subscribed topic NOT ending with /json will take a message as a plain string with no additional parameter. (hard coded default parameters will be used, in future configurable I hope).

It is also possible to use # for wildcard (at the end of a topic only), and + as part of a topic to indicate part of a topic path as a wildcard.

For example if you configure topic prefix as "rdadotmatrix/generic/#" you would get the following topic subscriptions:
```
Restoring MQTT connection...
ESP-MSG-ABCDEF connected to MQTT Server: 192.168.1.100:1883
Publishing to topic ESP-MSG-ABCDEF/status: connected
Subscribe to topic: rdadotmatrix
Subscribe to topic: rdadotmatrix/json
Subscribe to topic: rdadotmatrix/generic/#
Subscribe to topic: ESP-MSG-ABCDEF
Subscribe to topic: ESP-MSG-ABCDEF/json
```
Please Note: with a wildcard "#" at the end of the topic you would still be able to publish messages with parameters to a topic such as rdadotmatrix/generic/whatever/json or rdadotmatrix/generic/whatever/anotherlevel/json or any other longer multilevel topic.

## Send Messages using curl from cli:

```
curl --user admin:esp8266 -X POST http://192.168.1.89/api -H 'Content-Type: application/json' -d '{"MSG":"This is a test message","REP":"4","BUZ":"10","DEL":"30","BRI":"7","ASC":"1"}'
```

```
curl --user admin:esp8266 -X GET -G -s -o /dev/null 'http://192.168.1.89/arg' --data-urlencode "MSG=This is a test message" --data-urlencode "REP=10" --data-urlencode "BUZ=10" --data-urlencode "DEL=35" --data-urlencode "BRI=7" --data-urlencode "ASC=1"
```

```
curl --user admin:esp8266 -X GET -G -s -o /dev/null 'http://192.168.1.89/arg?MSG=This+is+a+test+message%21&REP=10&BUZ=10&DEL=35&BRI=7&ASC=1'
```

You can also use this URL encoded link fromatting to send messages from a browser:
```
http://192.168.1.89/arg?MSG=This+is+a+test+message%21&REP=10&BUZ=10&DEL=35&BRI=7&ASC=1
```
See https://meyerweb.com/eric/tools/dencoder/ for URL encode and decode 

## Send Messages from Home assistant Dashboard Card:

***Note: you'll have to use the base64 encoded as the username:password to send messages via HTTP***

You should configure the following in your secrets.yaml file if you are using default credentials:

```
dot_matrix_secret_header: "Basic YWRtaW46ZXNwODI2Ng=="
````

For any other you can calculate your own for example enter admin:esp8266 in http://n-cg.net/base64.htm and click encode to obtain YWRtaW46ZXNwODI2Ng==

You can for example define your own home assistant dashboard/lovelace interface to send test to the message board:

![home_assistant_gui_pannel](images/home_assistant_gui_pannel.jpg)

### Follow these steps:

#### 1. Create the relevant custom entities from home assistant gui and enter this in your lovelace interface tab for example:

```
type: entities
entities:
  - entity: input_text.dot_matrix_text
  - entity: input_select.dot_matrix_device_list
  - entity: input_text.dot_matrix_ip
  - entity: input_select.rda_dot_matrix_mqtt_topic
  - entity: input_number.dot_matrix_msg_repeat
  - entity: input_number.dot_matrix_buzzer
  - entity: input_number.dot_matrix_scroll_delay
  - entity: input_number.dot_matrix_brightness
  - entity: script.message_dot_matrix_http
  - entity: script.clear_dot_matrix_http
  - entity: script.message_dot_matrix_mqtt
  - entity: script.clear_dot_matrix_mqtt
title: Message Boards Texting (MAX7219)
```

#### 2. Enter this in configuration.yaml:
```
rest_command:
  message_dot_matrix_arg_http:
    url: "http://{{ states('input_text.dot_matrix_ip') }}/api"
    method: POST
    headers:
      authorization: !secret dot_matrix_secret_header
      content-type: "application/json"
    payload: "{MSG:'{{ states('input_text.dot_matrix_text') }}',REP:{{ states('input_number.dot_matrix_msg_repeat') }},BUZ:{{ states('input_number.dot_matrix_buzzer') }},DEL:{{ states('input_number.dot_matrix_scroll_delay') }},BRI:{{ states('input_number.dot_matrix_brightness') }},ASC:1}"
  clear_dot_matrix_arg_http:
    url: "http://{{ states('input_text.dot_matrix_ip') }}/arg"
    method: GET
    headers:
      authorization: !secret dot_matrix_secret_header
```

#### 3. if you have multiple message boards you can create an automations to switch IP address from the input_select list entity:

automation.change_value_of_dot_matrix_input_text
```
alias: Change Value of Dot Matrix Input Text
description: ''
trigger:
  - platform: state
    entity_id: input_select.dot_matrix_device_list
condition: []
action:
  - service: input_text.set_value
    target:
      entity_id: input_text.dot_matrix_ip
    data_template:
      value: >
        {% if is_state('input_select.dot_matrix_device_list', 'Living Room') %}
          192.168.1.88
        {% elif is_state('input_select.dot_matrix_device_list', 'Home Office')
        %}
          192.168.1.89
        {% endif %}
mode: single 
```

#### 4. Also create the following home assistant scripts:

script.message_dot_matrix:
```
sequence:
  - service: rest_command.message_dot_matrix_arg_http
    data: {}
mode: single
alias: Message Dot Matrix
```

script.clear_dot_matrix:
```
sequence:
  - service: rest_command.clear_dot_matrix_arg_http
    data: {}
mode: single
alias: Clear Dot Matrix
```

#### 5. For MQTT send message from home assistant, you can create the following script:

script.message_dot_matrix_mqtt
```
alias: Message Dot Matrix MQTT
sequence:
  - service: mqtt.publish
    data:
      topic: '{{ states(''input_select.rda_dot_matrix_mqtt_topic'') }}'
      payload: |-
        { MSG: "{{ states('input_text.dot_matrix_text') }}",
          REP: "{{ states('input_number.dot_matrix_msg_repeat') }}",
          BUZ: "{{ states('input_number.dot_matrix_buzzer') }}", 
          DEL: "{{ states('input_number.dot_matrix_scroll_delay') }}",
          BRI: "{{ states('input_number.dot_matrix_brightness') }}",
          ASC: '1' 
        }
mode: single
```

#### 6. To clear message repeats (stop displaying current message):

script.clear_dot_matrix_mqtt
```
alias: Clear Dot Matrix MQTT
sequence:
  - service: mqtt.publish
    data:
      topic: '{{ states(''input_select.rda_dot_matrix_mqtt_topic'') }}'
      payload: |-
        { MSG: ""
        }
mode: single
```
