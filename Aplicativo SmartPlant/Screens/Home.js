import { Text, View, StyleSheet, Image, Button } from 'react-native';
import Paho from 'paho-mqtt';

export default function Home({ value, temp, lumi, umis, umiar, espStatus, streamUrl }) {
  
  function changeValue(c) {
    const message = new Paho.Message((value + 1).toString());
    message.destinationName = 'mqtt-async-test/value';
    c.send(message);
  }

  return (
    <View>
      <Text style={styles.titulo}>Bem-vindo de volta</Text>

      <View style={styles.linha}>
        <View style={styles.quadrado}>
          <View style={styles.infoQuadrado}>
            <Image source={require('../assets/temperatura.png')} style={{width: 25, height: 25}}/>
            <Text>Temperatura</Text>
          </View>
          <Text style={styles.dadoQuadrado}>{temp}</Text>
          <Text>Graus Celsius</Text>
        </View>
        <View style={styles.quadrado}>
          <View style={styles.infoQuadrado}>
              <Image source={require('../assets/umidadeSolo.png')} style={{width: 40, height: 40}}/>
              <Text>Umidade do solo</Text>
          </View>
          <Text style={styles.dadoQuadrado}>{umis}%</Text>
        </View>
        
      </View>
      
      <View style={styles.linha}>
        <View style={styles.quadrado}>
        <View style={styles.infoQuadrado}>
            <Image source={require('../assets/luminosidade.png')} style={{width: 30, height: 30}}/>
            <Text>Luminosidade</Text>
          </View>
          <Text style={styles.dadoQuadrado}>{lumi}%</Text>
        </View>
        <View style={styles.quadrado}>
        <View style={styles.infoQuadrado}>
            <Image source={require('../assets/umidadeAr.png')} style={{width: 20, height: 20}}/>
            <Text>Umidade do ar</Text>
          </View>
          <Text style={styles.dadoQuadrado}>{umiar}%</Text>
        </View>
      </View>


      <View style={styles.teste}>
        <Button onPress={() => changeValue(client)} title={`Teste de conexão MQTT: ${value}`} color="#0AA085"/>
      </View>
      
    </View>
  );
}

const styles = StyleSheet.create({
  titulo: {
    fontWeight: "bold",
    fontSize: 20,
    padding: 10,
  },
  linha: {
    display: "flex",
    flexDirection: "row",
    padding: 10,
    justifyContent: "space-around",
  },
  quadrado: {
    display: "flex",
    alignItems: "center",
    justifyContent: "center",
    backgroundColor: "#FFF",
    width: 171,
    height: 208,
  },
  infoQuadrado: {
    display: "flex",
    flexDirection: "row",
    position: "absolute",
    top: 0,
    left: 0,
    padding: 10,
  },
  dadoQuadrado: {
    fontSize: 50,
  },
  teste: {
    display: "flex",
    flexDirection: "row",
    justifyContent: "center",
  }
});
