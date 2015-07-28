#28335 CANBUS

## About the project
This was made to work with converted MODBUS messages, so it has some similarities with MODBUS (example: the function codes). You can modify it to use with other kind of protocols as the function listen() and write() are very independent (just the process var is specific for this project).

## How to use

1. Define your NODE_ID at settings.h
2. Include can.h
3. Create a global variable called ´´´can_t can;´´´
4. Initialize the variable can before starting to use it with:
```
can = construct_can();
can.init(&can);
```
5. Put at some loop, or interruption, the following code:
```
can.clear();
can.listen(&can);
can.process(&can);
can.send(&can);
```
6. With the code above the CAN will be able to receive messages, process it and then send the reply
7. As can is global var, you can call it anywhere. Now, to define some value at the datamap you just call:
```
can.data.dummy0 = 20.5;
```
8. You can modify the datamap going to data.h and writing your own variables

## Contact

If you have some doubt about how the code works, contact me at contato@brunoluiz.net