const TelegramBot = require('node-telegram-bot-api');

let BrokerMQTT = 'mqtt://broker.shiftr.io';
let PuertoMQTT = 1883;
let ClienteIDMQTT = "ServidorToaster";
let UsuarioMQTT = "PaperBot";
let ContrasenaMQTT = "MAQUINADEGUERRAESLAONDA";
let ChatSecretoMQTT = "821249733";
let EstadodelHorno = false;
const token = '883954007:AAHf9AIxEPtA9UOy-8WTRXkuwogBdY6EO0I';

const bot = new TelegramBot(token, {
  polling: true
});

bot.on('message', (msg) => {
  const chatId = msg.chat.id;
  let message = msg.text;
  console.log("el id " + chatId);
  if (message == "Activar" || message == 'activar' || message == 'A') {
    client.publish('/toaster/Horno/mensaje', '1');
    bot.sendMessage(chatId, "Horno Activado");
    EstadodelHorno = true;
  } else if (message == "Desactivar" || message == 'desactivar' || message == 'D') {
    client.publish('/toaster/Horno/mensaje', '0');
    bot.sendMessage(chatId, "Horno Desactivado");
    EstadodelHorno = false;
  } else if (message == "Estado" || message == 'estado' || message == 'E') {
    client.publish('/toaster/Horno/tiempo', '1');
    bot.sendMessage(chatId, "Preguntando estado del Horno...");
  } else {
    bot.sendMessage(chatId, "Escribe bien que no te entiendo");
  };
});

let Opciones = {
  port: PuertoMQTT,
  clientId: ClienteIDMQTT,
  username: UsuarioMQTT,
  password: ContrasenaMQTT
}

var mqtt = require('mqtt')
var client = mqtt.connect(BrokerMQTT, Opciones);

client.on('connect', function() {
  client.subscribe('/toaster/#', function(err) {
    console.log("MQTT Activado")
  })
})

client.on('message', function(topic, message) {
  console.log("T: " + topic + "  M: " + message);
  let Mensaje = message.toString();
  if (EstadodelHorno) {
    if (topic == "/toaster/puerta/estado") {
      if (message == "1") {
        bot.sendMessage(ChatSecretoMQTT, "Peligro: Puerta Abierta");
      }
      else if(message == "3"){
        EstadodelHorno = false;
        bot.sendMessage(ChatSecretoMQTT, "Fin del proceso, retirar pagina");
      }
    }
  }
  if (topic == "/toaster/Horno/status") {
    if (message == "Apagado") {
      bot.sendMessage(ChatSecretoMQTT, "Horno apagado");
    } else {
      bot.sendMessage(ChatSecretoMQTT, "En proceso desde: " + message + " minutos.");
    }

  }
})
