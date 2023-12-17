import { useState, useEffect } from 'react';
import { StyleSheet, Text, View, Image } from 'react-native';
import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';
import { NavigationContainer } from '@react-navigation/native';
import Paho from 'paho-mqtt';

import Home from "./Screens/Home"
import Camera from "./Screens/Camera"

const Tab = createBottomTabNavigator();

client = new Paho.Client(
  'broker.hivemq.com',
  Number(8000),
  `estufa-tcc${parseInt(Math.random() * 100)}`
);

export default function App() {
  const [value, setValue] = useState(0);
  const [temp, setTemp] = useState(0);
  const [lumi, setLumi] = useState(0);
  const [umis, setUmis] = useState(0);
  const [umiar, setUmiar] = useState(0);
  const [espStatus, setEspStatus] = useState('OFF');
  const [streamUrl, setStreamUrl] = useState('');

  function onMessage(message) {
    if (message.destinationName === 'mqtt-async-test/value') setValue(parseInt(message.payloadString));
    if (message.destinationName === 'estufa-tcc/temp') setTemp(message.payloadString);
    if (message.destinationName === 'estufa-tcc/lumi') setLumi(message.payloadString);
    if (message.destinationName === 'estufa-tcc/umis') setUmis(message.payloadString);
    if (message.destinationName === 'estufa-tcc/umiar') setUmiar(message.payloadString);
    if (message.destinationName === 'estufa-tcc/status') setEspStatus('ON');
    if (message.destinationName === 'ifrj_cnit_estufatcc_ip_esp32')
      setStreamUrl(message.payloadString);
  }

  useEffect(() => {
    client.connect({
      onSuccess: () => {
        console.log('Connected!');
        client.subscribe('mqtt-async-test/value');
        client.subscribe('estufa-tcc/temp');
        client.subscribe('estufa-tcc/lumi');
        client.subscribe('estufa-tcc/umis');
        client.subscribe('estufa-tcc/umiar');
        client.subscribe('estufa-tcc/status');
        client.subscribe('ifrj_cnit_estufatcc_ip_esp32');
        client.onMessageArrived = onMessage;
      },
      onFailure: () => {
        console.log('Failed to connect!');
      },
    });
  }, []);

  useEffect(() => {
    const timer = setTimeout(() => {
      setEspStatus('OFF');
    }, 15000);

    return () => clearTimeout(timer);
  }, [espStatus]);

  return (
    <NavigationContainer>
      <Tab.Navigator
        tabBarOptions={{
          showLabel: false,
          activeTintColor: "#017f01",
        }}
      >
        <Tab.Screen name="Home"
          options={{
            tabBarIcon: ({ color }) => (
              <Image source={require('./assets/home.png')} style={{
                opacity: color == "#017f01" ? 1 : 0.5,
                width: 30,
                height: 30,
              }}/>
            ),
        }}>
          {() => (<Home value={value} temp={temp} lumi={lumi} umis={umis} umiar={umiar} espStatus={espStatus} streamUrl={streamUrl}/>)}
        </Tab.Screen>
        
        <Tab.Screen name="ON/OFF" component={Camera}
          options={{
            tabBarIcon: ({color}) => (
              <View style={styles.statusButton}>
                <Text style={styles.statusButtonText}>{espStatus}</Text>
              </View>
            )
          }}>
          {() => (
            <Camera value={value} temp={temp} lumi={lumi} umis={umis} umiar={umiar} espStatus={espStatus} streamUrl={streamUrl}/>
            )}
        </Tab.Screen>
        

        <Tab.Screen name="Câmera" 
          options={{
            tabBarIcon: ({ color }) => (
              <Image source={require('./assets/camera.png')} style={{
                opacity: color == "#017f01" ? 1 : 0.5,
                width: "30%",
                height: 30,
              }}/>
            ),
          }}> 
          {() => (
            <Camera value={value} temp={temp} lumi={lumi} umis={umis} umiar={umiar} espStatus={espStatus} streamUrl={streamUrl}/>
            )}
        </Tab.Screen>
      </Tab.Navigator>
    </NavigationContainer>
  );
}

const styles = StyleSheet.create({
  statusButton: {
    width: 60,
    height: 60,
    borderRadius: 36,
    backgroundColor: "#619373",
    marginBottom: 50,
    display: "flex",
    alignItems: "center",
    justifyContent: "center",
  },
  statusButtonText: {
    fontSize: 18,
    color: "white",
  }
});