import { Text, View, StyleSheet, Linking, TouchableHighlight, Image, Button } from 'react-native';
import Paho from 'paho-mqtt';

export default function Camera({ value, temp, lumi, umis, umiar, espStatus, streamUrl }) {
  
   const openStreamLink = async () => {
    // Verifique se o dispositivo pode abrir o URL
    const supported = await Linking.canOpenURL(streamUrl);

    if (supported) {
      // Abra o URL
      await Linking.openURL(streamUrl);
    } else {
      console.error(`Não é possível abrir o URL: ${streamUrl}`);
    }
  };

  return (
    <View style={styles.container}>
      <TouchableHighlight style={styles.button} onPress={openStreamLink}>
        <Text style={styles.buttonText}>Visualizar câmera</Text>
      </TouchableHighlight>
      <Text style={styles.text}>Endereço: {streamUrl}</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: "center",
  },
  button: {
    backgroundColor: "#0AA085",
    borderRadius: 8,
    padding: 14,
  },
  buttonText: {
    color: "white",
    fontSize: 22,
    fontWeight: "bold",
  },
  text: {
    color: "#2B2929",
    fontSize: 20,
    fontWeight: "bold",
    padding: 10,
  }
});
