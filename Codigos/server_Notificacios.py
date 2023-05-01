import Pyro4

import pymysql
from pymysql import  *
import pywhatkit
import time
import pyautogui
import keyboard as k



connection = pymysql.connect(
            host="localhost",
            user="root",
            passwd="",
            db="felix"
            )
@Pyro4.expose
class advertencias():





    def holaMundo(self):
        print("Hola Mundo")

    def recuperarAsistensias(self):

        cursor = connection.cursor()
        sql = "SELECT esudiante_idEsudiante FROM asistencia"
        cursor.execute(sql)
        registro = cursor.fetchall()
        return registro

    def recuperarAlumnosRegistrados(self):

        cursor = connection.cursor()
        sql = "SELECT esudiante_idEsudiante FROM esudiante_has_materia WHERE materia_idmateria = '6'"
        cursor.execute(sql)
        registro = cursor.fetchall()
        return registro

    def obtenerListas(self):
        asistencias = self.recuperarAsistensias()
        print(asistencias)
        listaAsistencia = []

        for i in range(len(asistencias)):
            listaAsistencia.append(asistencias[i][0])

        registros = self.recuperarAlumnosRegistrados()
        print(registros)
        listaRegistros = []

        for i in range(len(registros)):
            listaRegistros.append(registros[i][0])



        print("...." + str(listaAsistencia))
        for i in range(len(listaAsistencia)):
            #listaRegistros.remove(listaAsistencia[i])
            pass
        print(listaRegistros[0])
        return listaRegistros

    def recuperarTelefonos(self, id):

        cursor = connection.cursor()
        sql = "SELECT P.telefono FROM padrefamilia P INNER JOIN esudiante E " \
              "ON P.idPadreFamilia = E.padreFamilia_idEsudiante WHERE E.idEsudiante = '{}' ".format(id)
        cursor.execute(sql)
        registro = cursor.fetchall()

        return registro

    def enviarwats(self):
        print("Enviare el mensaje :D")
        # contact = '+527224495988'
        id = self.obtenerListas()
        #contact = self.recuperarTelefonos(id[0])

        contact2 = '+527224495988'
        hour = 2
        minute = 3
        # minute2 = 2
        message = '/archivo.pdf'

        # pywhatkit.sendwhatmsg_instantly(contact[0][0], message)
        #pywhatkit.sendwhatmsg(contact[0][0], message, hour, minute)
        #pywhatkit.sendwhatmsg(contact2, message, hour, minute)
        pywhatkit.sendwhats_image(contact2, "/dragon.jpg")
        pyautogui.click(1050, 950)

        time.sleep(2)
        k.press_and_release('enter')
    def notificar(self):
        self.enviarwats()
        return  "Envio de mensajes..."
deamon = Pyro4.Daemon()
uri = deamon.register(advertencias)

print(uri)

deamon.requestLoop()

#----------------------------------------------------------------------------------------------------------------------


