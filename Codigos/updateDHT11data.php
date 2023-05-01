<?php
  require 'database.php';
  
  if (!empty($_POST)) {

    $DatoHex = $_POST['DatoHex'];
    //Variables a insertar
    $feacha = "2023-03-24" ;
    $val1 = "1";  
    $materia_idmateria = "6" ;
    $esudiante_idEsudiante = "1" ;
    
    //........................................ Conectar con la base de datos
    $pdo = Database::connect();
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);


    //-----------------------------------------------------------
    $myObj = (object)array();

    //Validar existencia de un alumno con el RFID Detectado
    gh= "SELECT idEsudiante FROM esudiante WHERE rfid = '$DatoHex'";
    foreach ($pdo->query($sql) as $row) {
      $myObj->idEsudiante = $row['idEsudiante'];
    }
    //---------------------------------------------------------
    //Insertar datos en la tabla asistencias
    $sql = "INSERT INTO asistencia (feacha, val1, materia_idmateria, esudiante_idEsudiante) VALUES (?,?,?,?) ";
    $q = $pdo->prepare($sql);
    $q->execute(array($feacha, $val1, $materia_idmateria, $myObj->idEsudiante));
    Database::disconnect();
    //........................................ 
  }
  //---------------------------------------- 
?>