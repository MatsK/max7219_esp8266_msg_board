## Send messages from Node-Red

Import file node_red_flow.json into nodered.

Please Note: There are several subflows (look inside). 

Specific npm_packages will need to be installed in NodeRed to use this import and I believe these are all you need to add (hope I didn't miss any): 
```
npm_packages:
  - node-red-contrib-simple-message-queue
  - node-red-node-feedparser
```

**Note:** From the node-red import file node_red_flow.json (it uses MQTT) you'll find a sub-group with a function where I escape characters like backslash or double-quotes which can stop a message from displaying at all. This is particularly useful if working with RSS feeds in node-red.

### [Back to README.md](../README.md)
