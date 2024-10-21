import 'dart:convert';
import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';

enum ControlState { manual, semi_auto, auto }

enum GenOrGrid { gen, grid }

class ControlPage extends StatefulWidget {
  final BluetoothDevice server;

  const ControlPage({required this.server});

  @override
  _ControlPage createState() => new _ControlPage();
}

class _ControlPage extends State<ControlPage> {
  static final clientID = 0;
  BluetoothConnection? connection;
  String message = '';

  List<_Message> messages = List<_Message>.empty(growable: true);
  String _messageBuffer = '';

  final TextEditingController textEditingController =
      new TextEditingController();
  final ScrollController listScrollController = new ScrollController();

  bool isConnecting = true;
  bool get isConnected => (connection?.isConnected ?? false);
  bool isDisconnecting = false;
  ControlState? _controlState = ControlState.auto;
  GenOrGrid? _genOrGrid = GenOrGrid.grid;
  String controlState = 'automatic';
  String genOrGrid = 'grid';

  Map<String, bool> ledStatus = {
    "load_fail": false,
    "manual": false,
    "semi_auto": false,
    "fully_auto": false,
    "load_on": false,
    "gen_on": false,
    "gen_fail": false,
    "grid_on": false
  };

  bool isManual = false;
  bool isSemiAuto = false;
  bool isAuto = false;

  @override
  void initState() {
    super.initState();

    if (ledStatus['semi_auto'] == true) {
      _controlState = ControlState.semi_auto;
      controlState = 'semi automatic';
    }
    if (ledStatus['manual'] == true) {
      _controlState = ControlState.manual;
      controlState = 'manual';
    }
    if (ledStatus['fully_auto'] == true) {
      _controlState = ControlState.auto;
      controlState = 'automatic';
    }
    if (ledStatus["gen_on"] == true) {
      _genOrGrid = GenOrGrid.gen;
      genOrGrid = 'generator';
    }

    BluetoothConnection.toAddress(widget.server.address).then((_connection) {
      print('Connected to the device');
      connection = _connection;
      setState(() {
        isConnecting = false;
        isDisconnecting = false;
      });

      connection!.input!.listen(_onDataReceived).onDone(() {
        if (isDisconnecting) {
          print('Disconnecting locally!');
        } else {
          print('Disconnected remotely!');
        }
        if (this.mounted) {
          setState(() {});
        }
      });
    }).catchError((error) {
      print('Cannot connect, exception occurred');
      print(error);
    });
  }

  @override
  void dispose() {
    if (isConnected) {
      isDisconnecting = true;
      connection?.dispose();
      connection = null;
    }

    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final List<Row> list = messages.map((_message) {
      return Row(
        children: <Widget>[
          Container(
            child: Text(
                (text) {
                  return text == '/shrug' ? '¯\\_(ツ)_/¯' : text;
                }(_message.text.trim()),
                style: const TextStyle(color: Colors.white)),
            padding: const EdgeInsets.all(12.0),
            margin: const EdgeInsets.only(bottom: 8.0, left: 8.0, right: 8.0),
            width: 222.0,
            decoration: BoxDecoration(
                color:
                    _message.whom == clientID ? Colors.blueAccent : Colors.grey,
                borderRadius: BorderRadius.circular(7.0)),
          ),
        ],
        mainAxisAlignment: _message.whom == clientID
            ? MainAxisAlignment.end
            : MainAxisAlignment.start,
      );
    }).toList();

    final serverName = widget.server.name ?? "Unknown";
    return Scaffold(
      appBar: AppBar(
          title: (isConnecting
              ? Text('Connecting to $serverName...')
              : isConnected
                  ? Text('Connected to $serverName light control')
                  : Text('Disconnected from $serverName'))),
      body: SafeArea(
        child: Column(
          children: <Widget>[
            isConnecting
                ? Center(child: Text("Connecting to $serverName"))
                : isConnected
                    ? Flexible(
                        child: Padding(
                          padding: const EdgeInsets.all(12.0),
                          child: Container(
                            height: 400,
                            decoration: const BoxDecoration(
                                boxShadow: [
                                  BoxShadow(
                                      blurRadius: 2,
                                      spreadRadius: 2,
                                      color: Colors.grey)
                                ],
                                color: Colors.white,
                                borderRadius:
                                    BorderRadius.all(Radius.circular(12))),
                            child: Column(
                              crossAxisAlignment: CrossAxisAlignment.center,
                              mainAxisAlignment: MainAxisAlignment.spaceAround,
                              children: [
                                const Text("status"),
                                const SizedBox(height: 10),
                                Flexible(
                                  child: GridView.count(
                                    crossAxisCount: 3,
                                    children: [
                                      ...ledStatus.entries
                                          .where(
                                              (data) => data.key.contains("on"))
                                          .map((entry) {
                                        return GridTile(
                                          child: Column(
                                            crossAxisAlignment:
                                                CrossAxisAlignment.center,
                                            children: [
                                              Icon(Icons.lightbulb,
                                                  color: entry.value
                                                      ? Colors.yellow
                                                      : Colors.grey),
                                              Text('${entry.key}'
                                                  .split('_')
                                                  .join(' ')),
                                            ],
                                          ),
                                        );
                                      }).toList(),
                                    ],
                                  ),
                                ),
                                Flexible(
                                  child: GridView.count(
                                    crossAxisCount: 3,
                                    children: [
                                      ...ledStatus.entries
                                          .where((data) =>
                                              data.key.contains('fail'))
                                          .map((entry) {
                                        return GridTile(
                                          child: Column(
                                            crossAxisAlignment:
                                                CrossAxisAlignment.center,
                                            children: [
                                              if (entry.value == true)
                                                Icon(Icons.lightbulb,
                                                    color: Colors.red.shade800),
                                              if (entry.value == true)
                                                Text('${entry.key}'
                                                    .split("_")
                                                    .join(' '))
                                            ],
                                          ),
                                        );
                                      }).toList(),
                                    ],
                                  ),
                                ),
                                const SizedBox(height: 20),
                                const Text("modes"),
                                const SizedBox(height: 10),
                                Flexible(
                                  child: GridView.count(
                                    crossAxisCount: 3,
                                    children: [
                                      ...ledStatus.entries
                                          .where((element) =>
                                              element.key.contains("auto") ||
                                              element.key.contains("manual"))
                                          .map((entry) {
                                        return GridTile(
                                          child: Column(
                                            crossAxisAlignment:
                                                CrossAxisAlignment.center,
                                            children: [
                                              Icon(Icons.lightbulb,
                                                  color: entry.value
                                                      ? Colors.yellow
                                                      : Colors.grey),
                                              Text('${entry.key}'
                                                  .split("_")
                                                  .join(' ')),
                                            ],
                                          ),
                                        );
                                      }).toList(),
                                    ],
                                  ),
                                ),
                              ],
                            ),
                          ),
                        ),
                      )
                    : Center(child: Text("Disconnected from $serverName")),
            if (isConnected)
              Container(
                  child: Column(
                children: [
                  const Text("controls "),
                  SizedBox(
                    height: 10,
                  ),
                  Row(
                    children: [],
                  ),
                  Container(
                    margin: EdgeInsets.fromLTRB(16, 10, 16, 10),
                    decoration: const BoxDecoration(
                        boxShadow: [
                          BoxShadow(
                              blurRadius: 2,
                              spreadRadius: 2,
                              color: Colors.purple)
                        ],
                        color: Colors.white,
                        borderRadius: BorderRadius.all(Radius.circular(12))),
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.center,
                      mainAxisAlignment: MainAxisAlignment.spaceAround,
                      children: [
                        Row(
                          crossAxisAlignment: CrossAxisAlignment.center,
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            Text("system mode ",
                                style: TextStyle(
                                    color: Colors.grey,
                                    fontSize: 12,
                                    fontWeight: FontWeight.w500)),
                            Text(controlState,
                                style: TextStyle(
                                    color: Colors.black,
                                    fontSize: 14,
                                    fontWeight: FontWeight.w600))
                          ],
                        ),
                        RadioListTile<ControlState>(
                          title: const Text('automatic'),
                          value: ControlState.auto,
                          groupValue: _controlState,
                          onChanged: (ControlState? value) {
                            _sendMessage('auto');
                            setState(() {
                              _controlState = value;
                              controlState = 'automatic';
                            });
                          },
                        ),
                        RadioListTile<ControlState>(
                          title: const Text('manual'),
                          value: ControlState.manual,
                          groupValue: _controlState,
                          onChanged: (ControlState? value) {
                            _sendMessage('man');
                            setState(() {
                              _controlState = value;
                              controlState = 'manual';
                            });
                          },
                        ),
                        RadioListTile<ControlState>(
                          title: const Text('semi automatic'),
                          value: ControlState.semi_auto,
                          groupValue: _controlState,
                          onChanged: (ControlState? value) {
                            _sendMessage('semi');
                            setState(() {
                              _controlState = value;
                              controlState = 'semi automatic';
                            });
                          },
                        ),
                      ],
                    ),
                  ),
                  SizedBox(height: 30),
                  if (_controlState == ControlState.manual ||
                      _controlState == ControlState.semi_auto)
                    Container(
                      margin: EdgeInsets.fromLTRB(16, 10, 16, 10),
                      decoration: const BoxDecoration(
                          boxShadow: [
                            BoxShadow(
                                blurRadius: 2,
                                spreadRadius: 2,
                                color: Colors.purpleAccent)
                          ],
                          color: Colors.white,
                          borderRadius: BorderRadius.all(Radius.circular(12))),
                      child: Column(
                        crossAxisAlignment: CrossAxisAlignment.center,
                        mainAxisAlignment: MainAxisAlignment.spaceAround,
                        children: [
                          Row(
                            crossAxisAlignment: CrossAxisAlignment.center,
                            mainAxisAlignment: MainAxisAlignment.center,
                            children: [
                              Text("switch ",
                                  style: TextStyle(
                                      color: Colors.grey,
                                      fontSize: 12,
                                      fontWeight: FontWeight.w500)),
                              Text(genOrGrid,
                                  style: TextStyle(
                                      color: Colors.black,
                                      fontSize: 14,
                                      fontWeight: FontWeight.w600))
                            ],
                          ),
                          RadioListTile<GenOrGrid>(
                            title: const Text('grid'),
                            value: GenOrGrid.grid,
                            groupValue: _genOrGrid,
                            onChanged: (GenOrGrid? value) {
                              _sendMessage('grid');

                              setState(() {
                                _genOrGrid = value;
                                genOrGrid = 'grid';
                              });
                            },
                          ),
                          RadioListTile<GenOrGrid>(
                            title: const Text('generator'),
                            value: GenOrGrid.gen,
                            groupValue: _genOrGrid,
                            onChanged: (GenOrGrid? value) {
                              _sendMessage('gen');
                              setState(() {
                                _genOrGrid = value;
                                genOrGrid = 'generator';
                              });
                            },
                          ),
                        ],
                      ),
                    ),
                  ElevatedButton(
                      onPressed: () {
                        _sendMessage('stop');
                      },
                      child: Container(
                        width: 60,
                        child: const Row(
                          children: [Text('stop'), Icon(Icons.dangerous)],
                        ),
                      ))
                ],
              ))
          ],
        ),
      ),
    );
  }

  // void _onDataReceived(Uint8List data) {
  //   String dataString = String.fromCharCodes(data);
  //   _messageBuffer += dataString;

  //   int index;
  //   while ((index = _messageBuffer.indexOf('}')) != -1) {
  //     String message = _messageBuffer.substring(0, index).trim() + '}';
  //     print('this na the message  $message ');
  //     _messageBuffer = _messageBuffer.substring(index + 1);

  //     if (message.isNotEmpty) {
  //       try {
  //         Map<String, dynamic> jsonData = json.decode(message);
  //         setState(() {
  //           jsonData.forEach((key, value) {
  //             if (ledStatus.containsKey(key)) {
  //               ledStatus[key] = value;
  //             }
  //           });
  //         });
  //       } catch (e) {
  //         print('Error parsing JSON: $e');
  //       }
  //     }
  //   }
  // }
  void _onDataReceived(Uint8List data) {
    String dataString = String.fromCharCodes(data);
    _messageBuffer += dataString;

    int startIndex;
    int endIndex;

    while ((startIndex = _messageBuffer.indexOf('{')) != -1 &&
        (endIndex = _messageBuffer.indexOf('}', startIndex)) != -1) {
      String message =
          _messageBuffer.substring(startIndex, endIndex + 1).trim();
      print('this is the message $message');
      _messageBuffer = _messageBuffer.substring(endIndex + 1);

      if (message.isNotEmpty) {
        try {
          Map<String, dynamic> jsonData = json.decode(message);
          setState(() {
            jsonData.forEach((key, value) {
              if (ledStatus.containsKey(key)) {
                ledStatus[key] = value;
              }
            });
            if (ledStatus['semi_auto'] == true) {
              _controlState = ControlState.semi_auto;
              controlState = 'semi automatic';
            }
            if (ledStatus['manual'] == true) {
              _controlState = ControlState.manual;
              controlState = 'manual';
            }
            if (ledStatus['fully_auto'] == true) {
              _controlState = ControlState.auto;
              controlState = 'automatic';
            }
            if (ledStatus["gen_on"] == true) {
              _genOrGrid = GenOrGrid.gen;
              genOrGrid = 'generator';
            }
          });
        } catch (e) {
          print('Error parsing JSON: $e');
        }
      }
    }

    // Clean up _messageBuffer to remove any partial message not starting with '{'
    startIndex = _messageBuffer.indexOf('{');
    if (startIndex != -1) {
      _messageBuffer = _messageBuffer.substring(startIndex);
    } else {
      _messageBuffer = '';
    }
  }

  void _toggleLed(String led) async {
    String command = ledStatus[led]! ? 'off' : 'on';
    _sendMessage('$led:$command');
  }

  void _sendMessage(String text) async {
    text = text.trim();
    textEditingController.clear();

    if (text.isNotEmpty) {
      try {
        connection!.output.add(Uint8List.fromList(utf8.encode(text + "\r\n")));
        await connection!.output.allSent;

        setState(() {
          messages.add(_Message(clientID, text));
        });

        // Future.delayed(const Duration(milliseconds: 333)).then((_) {
        //   listScrollController.animateTo(
        //       listScrollController.position.maxScrollExtent,
        //       duration: const Duration(milliseconds: 333),
        //       curve: Curves.easeOut);
        // }
        // );
      } catch (e) {
        setState(() {});
      }
    }
  }
}

class _Message {
  int whom;
  String text;

  _Message(this.whom, this.text);
}
