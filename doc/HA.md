## Home Assistant Feedreader

You can also use home assistant RSS Feeds to send news text to the message board or even better NodeRed (which I prefer for this, included in the node-red import file node_red_flow.json)

#### 1. enter some rss feeds like the below for example in your home assistant configuration.yaml
```
feedreader:
  urls:
    - http://feeds.bbci.co.uk/news/world/rss.xml
    - http://feeds.bbci.co.uk/news/rss.xml
    - http://feeds.bbci.co.uk/news/health/rss.xml
    - http://feeds.bbci.co.uk/news/technology/rss.xml
    - http://feeds.bbci.co.uk/news/science_and_environment/rss.xml
  scan_interval:
    minutes: 5
  max_entries: 5
```

And an http call with arguments or json api again in configuration.yaml

```
rest_command:
  ### some examples althought we'll use the first one for RSS feed automation below
  feed_to_dot_matrix_api_http:
    url: "http://{{ipaddress}}/api"
    method: POST
    headers:
      authorization: !secret dot_matrix_secret_header
      content-type: "application/json"
    payload: "{MSG:'{{msg}}',REP:{{rep}},BUZ:{{buz}},DEL:{{del}},ASC:1}"
  feed_to_dot_matrix_arg_http:
    url: "http://{{ipaddress}}/arg?MSG={{msg}}&REP={{rep}}&BUZ={{buz}}&DEL={{del}}&BRI={{bri}}&ASC=1"
    method: GET
    headers:
      authorization: !secret dot_matrix_secret_header
```

Or you can use MQTT.

**Note:** it is important you use /json at the end of the topic.

Topics ending with /json are automatically subscribed regardless of the configure topic prefix.

Configure the following script:
```
alias: Feed To Dot Matrix MQTT
sequence:
- service: mqtt.publish
  data:
	topic: 'rdadotmatrix/json'
	payload: |-
		{ MSG: "{{msg}}",
		  REP: "{{rep}}",
		  BUZ: "{{buz}}", 
		  DEL: "{{del}}",
      DEL: "{{bri}}",
		  ASC: '1' 
		}
mode: single
```

#### 2. Then configure an automation as follows if you have for example 2 different message boards and you want to send to both:
```
alias: Feed Reader To Matrix via HTTP
description: ''
trigger:
  - platform: event
    event_type: feedreader
condition: []
action:
  - service: rest_command.feed_to_dot_matrix_api_http
    data:
      ipaddress: 192.168.1.88
      msg: >-
        News Feed: {{ trigger.event.data.title }}.... {{
        trigger.event.data.summary }}
      rep: 10
      buz: 10
      del: 35
      bri: 7
  - service: rest_command.feed_to_dot_matrix_api_http
    data:
      ipaddress: 192.168.1.89
      msg: >-
        News Feed: {{ trigger.event.data.title }}.... {{
        trigger.event.data.summary }}
      rep: 10
      buz: 10
      del: 35
      bri: 7
mode: single
```

or if you are using MQTT (not tested but should work)

```
alias: Feed Reader To Matrix via MQTT
description: ''
trigger:
  - platform: event
    event_type: feedreader
condition: []
action:
  - service: script.feed_to_dot_matrix_mqtt
    data:
      msg: >-
        News Feed: {{ trigger.event.data.title }}.... {{
        trigger.event.data.summary }}
      rep: 10
      buz: 10
      del: 35
      bri: 7
mode: single
```
