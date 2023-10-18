import Paho from "paho-mqtt"; //Importa biblioteca Paho

import { useState, useEffect } from "react";
import { StatusBar } from 'expo-status-bar';
import { StyleSheet, Text, Button, View } from 'react-native';

client = new Paho.Client(
  "broker.hivemq.com", // endereço do broker
  Number(8000), //porta Websocket 
  `estufa-tcc${parseInt(Math.random() * 100)}` // id do app no broker
);

export default function App() {

  const [value, setValue] = useState(0);
  const [temp, setTemp] = useState(0); // Variavel que guarda a temperatura

  function onMessage(message) { //variavel que lê mensagens recebidas
    if (message.destinationName === "mqtt-async-test/value") //caso a mensagem vier do topico mqtt-async-test/value vai mandar ela pra variavel setValue
        setValue(parseInt(message.payloadString)); 
    if (message.destinationName === "mqtt-async-test/temp")
        setTemp((message.payloadString));
  }

  useEffect(() => {
    client.connect( {
      onSuccess: () => { 
      console.log("Connected!");
      client.subscribe("mqtt-async-test/value");
      client.subscribe("mqtt-async-test/temp"); //Topico que ele se inscreveu (serve para ler mensagens do topico)

      client.onMessageArrived = onMessage;
    },
    onFailure: () => {
      console.log("Failed to connect!");
    }
  });
  }, [])

  function changeValue(c) {
    const message = new Paho.Message((value + 1).toString());
    message.destinationName = "mqtt-async-test/value"; // Topico que ele vai enviar a mensagem
    c.send(message);
  }
  function mudarTemp(c) {
    const message = new Paho.Message;
    message.destinationName = "mqtt-async-test/temp";
    c.send(message);
  }


  return (
    <View style={styles.container}>
      <Text>Value is: {value}</Text>
      <Text>Temperatura is: {temp}</Text>

      <Button onPress={() => { changeValue(client);} } title="Press Me"/>

      <StatusBar style="auto" />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#fff',
    alignItems: 'center',
    justifyContent: 'center',
  },
});