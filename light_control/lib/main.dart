import 'package:flutter/material.dart';
import 'package:light_control/pages/main_page.dart';

void main() => runApp(const ExampleApplication());

class ExampleApplication extends StatelessWidget {
  const ExampleApplication({super.key});

  @override
  Widget build(BuildContext context) {
    return const MaterialApp(home: MainPage());
  }
}
